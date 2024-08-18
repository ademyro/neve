#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "compiler.h"
#include "ctx.h"
#include "emit.h"
#include "err.h"
#include "opt.h"
#include "pretty.h"
#include "tok.h"
#include "ir.h"

#ifdef DEBUG_COMPILE
#include "debug.h"
#endif

Parser newParser() {
  Tok nothing = emptyTok();

  Parser parser = {
    .curr = nothing,
    .prev = nothing,
    .isPanicking = false,
  };

  return parser;
}

static void markErr(Ctx *ctx) {
  ctx->parser.isPanicking = true;
}

static void unexpectedToken(Ctx *ctx, Tok tok) {
  CHECK_PANIC(ctx);
  markErr(ctx);

  char start = *tok.lexeme;

  if (start == '"') {
    // TODO: find a way to find the loc where the string begins
    setNewErr(&ctx->errMod, ERR_INFINITE_STR, tok.loc);
    ErrMod mod = ctx->errMod;

    reportErr(mod, "unterminated string");
    showOffendingLine(mod, "...  until end of file");
    showHint(mod, "you can insert a ‘\"’ where you missed it"); 

    endErr(mod);
  } else {
    setNewErr(&ctx->errMod, ERR_INVALID_CHAR, tok.loc);
    ErrMod mod = ctx->errMod;

    reportErr(mod, "unexpected character");
    showOffendingLine(mod, "here");
    showHint(mod, "‘%c’ isn’t recognized as a valid token", start);

    endErr(mod);
  }
}

static void unaryNegationErr(Ctx *ctx, Tok op, Tok tok, Node *operand) {
  CHECK_PANIC(ctx);
  markErr(ctx);

  Loc loc = mergeLocs(op.loc, tok.loc);

  setNewErr(&ctx->errMod, ERR_UNAPPLICABLE_OP, loc);
  ErrMod mod = ctx->errMod;

  reportErr(
    mod, 
    "cannot negate ‘%.*s’ of type ‘%s’", 
    SHOW_LEXEME(tok), 
    operand->valType.name
  );

  showOffendingLine(mod, "can’t negate ‘%.*s’", SHOW_LEXEME(tok));

  showHint(
    mod, 
    "you need to overload the unary ‘%.*s’ operator to apply it",
    SHOW_LEXEME(op)
  );

  endErr(mod);
}

static void binOpTypeErr(Ctx *ctx, Node *left, Tok op, Node *right) {
  CHECK_PANIC(ctx);
  markErr(ctx);

  Loc leftLoc = getFullLoc(left);
  Loc rightLoc = getFullLoc(right);
  Loc loc = op.loc;

  setNewErr(&ctx->errMod, ERR_UNAPPLICABLE_OP, loc); 
  ErrMod mod = ctx->errMod;
  
  // yeah this is kind of silly but
  const char *actionName;
  switch (op.type) {
    case TOK_PLUS:
      actionName = "sum";
      break;
    
    case TOK_MINUS:
      actionName = "subtract";
      break;
    
    case TOK_STAR:
      actionName = "multiply";
      break;

    case TOK_SLASH:
      actionName = "divide";
      break;

    default:
      actionName = "compare";
      break;
  }

  reportErr(
    mod, 
    "cannot %s ‘%s’ and ‘%s’", 
    actionName, 
    left->valType.name, 
    right->valType.name
  );

  showNote(mod, leftLoc, "%s", left->valType.name);
  showNote(mod, rightLoc, "%s", right->valType.name);

  endErr(mod);
}

static void advance(Ctx *ctx) {
  Parser *parser = &ctx->parser;
  parser->prev = parser->curr;
  
  while (true) {
    parser->curr = nextTok(&ctx->lexer);

    if (parser->curr.type == TOK_NEWLINE) {
      continue;
    }

    if (parser->curr.type != TOK_ERR) {
      break;
    }

    unexpectedToken(ctx, parser->curr);
  }
}

static bool isAtEnd(Ctx *ctx) {
  return ctx->parser.curr.type == TOK_EOF;
}

static Tok offendingTok(Ctx *ctx) {
  if (isAtEnd(ctx)) {
    return ctx->parser.prev;
  }

  return ctx->parser.curr;
}

void expect(Ctx *ctx, TokType type, const char *tokName) {
  Tok curr = ctx->parser.curr;

  if (curr.type == type) {
    advance(ctx);
    return;
  }

  CHECK_PANIC(ctx);

  markErr(ctx);

  setNewErr(&ctx->errMod, ERR_SYNTAX, curr.loc);
  ErrMod mod = ctx->errMod;

  reportErr(mod, "unexpected symbol");

  showOffendingLine(
    mod, 
    "expected %s, but found ‘%.*s’", 
    tokName, 
    SHOW_LEXEME(curr)
  );

  showHint(
    mod, 
   "no worries!  you just need to replace it "
   "with the expected character."
  );

  /*
  // TODO: replace [] with the official repository at some point.
  showHint(
    mod,
    "or it might be a parser bug.  in which case, be sure "
    "to report it at []!"
  );
  */

  endErr(mod);
}

static Tok consume(Ctx *ctx) {
  Tok tok = ctx->parser.curr;
  advance(ctx);

  return tok;
}

static bool check(Ctx *ctx, TokType type) {
  return ctx->parser.curr.type == type;
}

static bool checkEither(Ctx *ctx, TokType one, TokType two) {
  return check(ctx, one) || check(ctx, two);
}

static bool match(Ctx *ctx, TokType type) {
  if (!check(ctx, type)) {
    return false;
  }

  advance(ctx);
  return true;
}

static void endCompiler(Ctx *ctx) {
  Tok curr = ctx->parser.curr;

  freeTypeTable(ctx->types);

  emitReturn(ctx, curr.loc);

#ifdef DEBUG_COMPILE
  if (ctx->errMod.errCount == 0) {
    disasmChunk(currChunk(ctx), "code");
  }
#endif
}

static Node *expr(Ctx *ctx);
static Node *equality(Ctx *ctx);
static Node *comparison(Ctx *ctx);
static Node *term(Ctx *ctx);
static Node *factor(Ctx *ctx);
static Node *unary(Ctx *ctx);
static Node *primary(Ctx *ctx);
static Node *intLiteral(Ctx *ctx);
static Node *floatLiteral(Ctx *ctx);

static Node *expr(Ctx *ctx) {
  return equality(ctx);
}

static Node *equality(Ctx *ctx) {
  Node *left = comparison(ctx); 

  while (checkEither(ctx, TOK_EQUAL, TOK_NEQUAL)) {
    Tok op = consume(ctx);

    Node *right = comparison(ctx);

    Node *binOp = newBinOp(ctx->types, left, op, right);
    left = binOp; 
  }

  return left;
}

static Node *comparison(Ctx *ctx) {
  Node *left = term(ctx); 

  while (
    checkEither(ctx, TOK_LESS, TOK_GREATER) ||
    checkEither(ctx, TOK_LESS_EQUAL, TOK_GREATER_EQUAL)
  ) {
    Tok op = consume(ctx);

    Node *right = term(ctx);

    if (!isNum(left) || !isNum(right)) {
      binOpTypeErr(ctx, left, op, right);
    }

    Node *binOp = newBinOp(ctx->types, left, op, right);
    left = binOp; 
  }

  return left;
}

static Node *term(Ctx *ctx) {
  Node *left = factor(ctx); 

  while (checkEither(ctx, TOK_PLUS, TOK_MINUS)) {
    Tok op = consume(ctx);

    Node *right = factor(ctx);

    if (!isNum(left) || !isNum(right)) {
      binOpTypeErr(ctx, left, op, right);
    }

    Node *binOp = newBinOp(ctx->types, left, op, right);
    left = binOp; 
  }

  return left;
}

static Node *factor(Ctx *ctx) {
  Node *left = unary(ctx); 

  while (checkEither(ctx, TOK_STAR, TOK_SLASH)) {
    Tok op = consume(ctx);

    Node *right = unary(ctx);

    if (!isNum(left) || !isNum(right)) {
      binOpTypeErr(ctx, left, op, right);
    }

    Node *binOp = newBinOp(ctx->types, left, op, right);
    left = binOp; 
  }

  return left;
}

static Node *unary(Ctx *ctx) {
  if (!checkEither(ctx, TOK_MINUS, TOK_NOT)) {
    return primary(ctx);
  }

  Tok op = consume(ctx); 
  Node *operand = unary(ctx);

  switch (op.type) {
    case TOK_MINUS: {
      // TODO: make this more robust once we implement classes--
      // allow for operator overloading and replace this check
      if (!isNum(operand)) {
        Tok tok = ctx->parser.prev;

        unaryNegationErr(ctx, op, tok, operand);
      }

      return newUnOp(ctx->types, op, UNOP_NEG, operand);
    }
    
    case TOK_NOT: {
      if (!checkType(operand, TYPE_BOOL) && !checkType(operand, TYPE_NIL)) {
        Tok tok = ctx->parser.prev;

        unaryNegationErr(ctx, op, tok, operand);
      }

      return newUnOp(ctx->types, op, UNOP_NEG, operand);
    }

    default:
      return primary(ctx);
  }
}

static Node *primary(Ctx *ctx) {
  // TODO: make this a switch the moment we add one more if 
  // statement
  if (check(ctx, TOK_INT)) {
    return intLiteral(ctx);
  }

  if (check(ctx, TOK_FLOAT)) {
    return floatLiteral(ctx);
  }

  if (checkEither(ctx, TOK_TRUE, TOK_FALSE)) {
    Tok tok = consume(ctx);
    
    return newBool(ctx->types, tok.type == TOK_TRUE, tok.loc);
  }

  if (check(ctx, TOK_NIL)) {
    Tok tok = consume(ctx);

    return newNil(ctx->types, tok.loc);
  }

  if (match(ctx, TOK_LPAREN)) {
    // TODO: maybe we’ll need a separate NODE_GROUPED variant
    // for constant folding?
    Node *grouped = expr(ctx);

    if (!match(ctx, TOK_RPAREN) && !IS_PANICKING(ctx)) {
      markErr(ctx);
      Tok curr = offendingTok(ctx);

      setNewErr(&ctx->errMod, ERR_OPEN_PARENS, curr.loc);
      ErrMod mod = ctx->errMod;

      reportErr(mod, "parentheses left open");
      showOffendingLine(mod, "expected ‘)’ to close parentheses");
      showHint(mod, "you can easily close them like so:");
      suggestFix(mod, curr.loc, ")");

      endErr(mod);
    }

    return grouped;
  }

  Tok curr = offendingTok(ctx);

  if (IS_PANICKING(ctx)) {
    // TODO: replace this with a `nil` node
    return newInt(ctx->types, -1L, curr.loc);
  }

  markErr(ctx);

  setNewErr(&ctx->errMod, ERR_INVALID_EXPR, curr.loc);
  ErrMod mod = ctx->errMod;

  reportErr(mod, "expected an expression");

  showOffendingLine(
    mod, 
    "expected an expression, but found ‘%.*s’", 
    SHOW_LEXEME(curr)
  );

  endErr(mod);

  // TODO: replace this with a `nil` node
  return newInt(ctx->types, -1L, curr.loc);
}

static Node *intLiteral(Ctx *ctx) {
  Tok integer = consume(ctx);
  
  // TODO: allow hexadecimal, binary, and octal sometime.
  const int base = 10;
  const long value = strtol(integer.lexeme, NULL, base);

  if ((value == LONG_MIN || value == LONG_MAX) && !IS_PANICKING(ctx)) {
    markErr(ctx);

    // TODO: use long longs at some point
    setNewErr(&ctx->errMod, ERR_INTEGER_OUT_OF_RANGE, integer.loc);
    ErrMod mod = ctx->errMod;

    reportErr(mod, "integer out of range (too big)");
    showOffendingLine(mod, "neve does not support integers this size yet");
    showHint(mod, "you can sum it in two parts to work around this issue:");

    Loc fixLoc = {
      .col = integer.loc.col + (int)integer.loc.length,
      .length = integer.loc.length,
      .line = integer.loc.line
    };

    suggestFix(mod, fixLoc, " / 2 + %.*s / 2", SHOW_LEXEME(integer));

    endErr(mod);
  }

  return newInt(ctx->types, value, integer.loc);
}

static Node *floatLiteral(Ctx *ctx) {
  Tok f = consume(ctx);

  const double value = strtod(f.lexeme, NULL);

  return newFloat(ctx->types, value, f.loc);
}

bool compile(const char *fname, const char *src, Chunk *ch) {
  IGNORE(emitBoth);

  ErrMod mod = newErrMod(fname, src);
  Ctx ctx = newCtx(mod, ch);

  advance(&ctx);
  Node *ast = expr(&ctx);
  expect(&ctx, TOK_EOF, "end of file");

  ErrMod newMod = ctx.errMod;

  const bool hadErrs = newMod.errCount != 0;

#ifdef DEBUG_COMPILE
  if (!hadErrs) {
    fprintf(stderr, "unoptimized:\n");
    prettyPrint(ast);
  }
#endif

  optNode(ast);

#ifdef DEBUG_COMPILE
  if (!hadErrs) {
    fprintf(stderr, "optimized:\n");
    prettyPrint(ast);
  }
#endif

  emitNode(&ctx, ast);

  freeNode(ast);

  endCompiler(&ctx);

  if (hadErrs) {
    cliErr("compilation failed due to %d previous errors", newMod.errCount);
  }

  return !hadErrs;
}
