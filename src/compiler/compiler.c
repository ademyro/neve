#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "compiler.h"
#include "ctx.h"
#include "err.h"
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

static Chunk *currChunk(Ctx *ctx) {
  return ctx->currCh;
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

static void emit(Ctx *ctx, uint8_t byte, Loc loc) {
  writeChunk(currChunk(ctx), byte, loc.line);
}

static void emitBoth(Ctx *ctx, uint8_t one, uint8_t two, Loc loc) {
  emit(ctx, one, loc);
  emit(ctx, two, loc);
}

static void emitReturn(Ctx *ctx, Loc loc) {
  emit(ctx, OP_RET, loc);
}

static void emitConst(Ctx *ctx, Val val, Loc loc) {
  writeConst(currChunk(ctx), val, loc.line);
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
  // not implemented yet
  return comparison(ctx); 
}

static Node *comparison(Ctx *ctx) {
  // not implemented yet
  return term(ctx);
}

static Node *term(Ctx *ctx) {
  Node *left = factor(ctx); 

  while (checkEither(ctx, TOK_PLUS, TOK_MINUS)) {
    Tok op = consume(ctx);

    Node *right = factor(ctx);

    emit(ctx, op.type == TOK_PLUS ? OP_ADD : OP_SUB, op.loc);
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

    emit(ctx, op.type == TOK_STAR ? OP_MUL : OP_DIV, op.loc);
    Node *binOp = newBinOp(ctx->types, left, op, right);
    left = binOp; 
  }

  return left;
}

static Node *unary(Ctx *ctx) {
  if (checkEither(ctx, TOK_NOT, TOK_MINUS)) {
    Tok op = consume(ctx); 

    Node *operand = unary(ctx);

    // TODO: unary boolean negation is not supported yet,
    // once it is, replace this with a ternary operator
    emit(ctx, OP_NEG, op.loc);

    return newUnOp(ctx->types, op, operand);
  }

  return primary(ctx);
}

static Node *primary(Ctx *ctx) {
  if (check(ctx, TOK_INT)) {
    return intLiteral(ctx);
  }

  if (check(ctx, TOK_FLOAT)) {
    return floatLiteral(ctx);
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

  emitConst(ctx, (double)value, integer.loc);
  return newInt(ctx->types, value, integer.loc);
}

static Node *floatLiteral(Ctx *ctx) {
  Tok f = consume(ctx);

  const double value = strtod(f.lexeme, NULL);

  emitConst(ctx, value, f.loc);
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
    prettyPrint(ast);
  }
#endif

  freeNode(ast);

  endCompiler(&ctx);

  if (hadErrs) {
    cliErr("compilation failed due to %d previous errors", newMod.errCount);
  }

  return !hadErrs;
}
