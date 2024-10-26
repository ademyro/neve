#include <string.h>

#include "lexer.h"

Lexer newLexer(const char *src) {
  Lexer lexer = {
    .start = src,
    .curr = src,
    .loc = newLoc(),
    .interpolDepth = 0
  };

  return lexer;
}

static bool isInRange(const int i, const int begin, const int end) {
  return i >= begin && i <= end;
}

static bool isDigit(const char c) {
  return isInRange(c, '0', '9');
}

static bool isAlpha(const char c) {
  return (
    isInRange(c, 'a', 'z')
    ||
    isInRange(c, 'A', 'Z')
    || 
    c == '_'
  );
}

static bool isAtEnd(Lexer *lexer) {
  return *lexer->curr == '\0';
}

static void sync(Lexer *lexer) {
  syncLoc(&lexer->loc);

  lexer->start = lexer->curr;
}

static void newline(Lexer *lexer) {
  newlineLoc(&lexer->loc);
}

static char advance(Lexer *lexer) {
  advanceLoc(&lexer->loc);
  return *lexer->curr++;
}

static char peek(Lexer *lexer) {
  return *lexer->curr;
} 

static char peekAhead(Lexer *lexer) {
  if (isAtEnd(lexer)) {
    return '\0';
  }

  return lexer->curr[1];
}

static bool match(Lexer *lexer, char c) {
  if (isAtEnd(lexer)) {
    return false; 
  }

  if (peek(lexer) != c) {
    return false;
  }

  advance(lexer);
  return true;
}

static bool checkSeq(Lexer *lexer, const char *seq) {
  size_t seqSize = strlen(seq);

  if (seqSize != 2) {
    return false;
  }

  return peek(lexer) == seq[0] && peekAhead(lexer) == seq[1];
}

static Tok makeTok(Lexer *lexer, TokType type) {
  Tok tok = newTok(
    lexer->start,
    type,
    lexer->loc
  );

  return tok;
}

static Tok unexpectedChar(Lexer *lexer) {
  return makeTok(lexer, TOK_ERR);
}

static void skipComment(Lexer *lexer) {
  while (peek(lexer) != '\n' && !isAtEnd(lexer)) {
    advance(lexer);
  }
}

static void skipWs(Lexer *lexer) {
  while (true) {
    char c = peek(lexer);

    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance(lexer);
        break;

      case '#':
        skipComment(lexer); 
        break;

      default:
        return;
    }
  }
}

static TokType checkKeyword(
  Lexer *lexer, 
  const int start, 
  const size_t length, 
  const char* rest, 
  TokType type
) {
  const size_t keywordLength = lexer->loc.length;
  // constantly having to do this is kinda annoying?  but we'll deal with it
  const size_t idLength = (size_t)start + length;
  
  if (
    keywordLength == idLength
    &&
    memcmp(lexer->start + start, rest, length) == 0
  ) {
    return type;
  }

  return TOK_ID;
}

static TokType checkForE(Lexer *lexer) {
  const size_t idLength = (size_t)(lexer->curr - lexer->start);

  switch (lexer->start[1]) {
    case 'l': 
      return checkKeyword(lexer, 2, 2, "se", TOK_ELSE);

    case 'n': 
      if (idLength > 2) {
        switch (lexer->start[2]) {
          case 'd':
            return checkKeyword(lexer, 3, 0, "", TOK_END);

          case 'u':
            return checkKeyword(lexer, 3, 1, "m", TOK_ENUM);
        }
      } 
      break;
  }

  return TOK_ID;
}

static TokType checkForF(Lexer *lexer) {
  switch (lexer->start[1]) {
    case 'a': 
      return checkKeyword(lexer, 2, 3, "lse", TOK_FALSE);

    case 'o': 
      return checkKeyword(lexer, 2, 1, "r", TOK_FOR);

    case 'u': 
      return checkKeyword(lexer, 2, 1, "n", TOK_FUN);
  }

  return TOK_ID;
}

static TokType checkForL(Lexer *lexer) {
  switch (lexer->start[1]) {
    case 'o':
      return checkKeyword(lexer, 2, 1, "g", TOK_LOG);

    case 'e':
      return checkKeyword(lexer, 2, 1, "t", TOK_LET);
  }

  return TOK_ID;
}

static TokType checkForN(Lexer *lexer) {
  switch (lexer->start[1]) {
    case 'i':
      return checkKeyword(lexer, 2, 1, "l", TOK_NIL);

    case 'o':
      return checkKeyword(lexer, 2, 1, "t", TOK_NOT);
  }

  return TOK_ID;
}

static TokType checkForW(Lexer *lexer) {
  switch (lexer->start[1]) {
    case 'h':
      return checkKeyword(lexer, 2, 3, "ile", TOK_WHILE);

    case 'i':
      return checkKeyword(lexer, 2, 2, "th", TOK_WITH);
  }

  return TOK_ID;
}

static TokType idType(Lexer *lexer) {
  const size_t idLength = (size_t)(lexer->curr - lexer->start);

  // i know this is weird, but if i don't declare a constant
  // like this, clang-tidy will yell at me because '5' is a magic number.
  // so i'm using returnLength - 1 instead.
  const size_t returnLength = 6;
  
  switch (*lexer->start) {
    case 'a': 
      return checkKeyword(lexer, 1, 2, "nd", TOK_AND);

    case 'c': 
      return checkKeyword(lexer, 1, 4, "lass", TOK_CLASS);

    case 'd':
      return checkKeyword(lexer, 1, 1, "o", TOK_DO);

    case 'e': 
      if (idLength > 1) {
        return checkForE(lexer);
      }

      break;

    case 'f':
      if (idLength > 1) {
        return checkForF(lexer);
      }

      break;

    case 'i': 
      return checkKeyword(lexer, 1, 1, "f", TOK_IF);

    case 'l': 
      if (idLength > 1) {
        return checkForL(lexer);
      }

      break;

    case 'm':
      return checkKeyword(lexer, 1, 4, "atch", TOK_MATCH);

    case 'n': 
      if (idLength > 1) {
        return checkForN(lexer);
      } 

      break;

    case 'o': 
      return checkKeyword(lexer, 1, 1, "r", TOK_OR);

    case 'r': 
      return checkKeyword(lexer, 1, returnLength - 1, "eturn", TOK_RETURN);

    case 't':
      return checkKeyword(lexer, 1, 3, "rue", TOK_TRUE);

    case 'v': 
      return checkKeyword(lexer, 1, 2, "ar", TOK_VAR);

    case 'w': 
      if (idLength > 1) {
        return checkForW(lexer);
      }

      break;
  }

  return TOK_ID;
}

static Tok id(Lexer *lexer) {
  while (isAlpha(peek(lexer)) || isDigit(peek(lexer))) {
    advance(lexer);
  }

  return makeTok(lexer, idType(lexer));
}

static Tok number(Lexer *lexer) {
  while (isDigit(peek(lexer))) {
    advance(lexer); 
  }

  const bool isFloat = peek(lexer) == '.' && isDigit(peekAhead(lexer));

  if (isFloat) {
    advance(lexer);

    while (isDigit(peek(lexer))) {
      advance(lexer);
    }
  }

  return makeTok(lexer, isFloat ? TOK_FLOAT : TOK_INT);
}

static Tok interpol(Lexer *lexer) {
  // FIXME: prevent interpolDepth from exceeding 255, so we can encode the 
  // number of interpolations in a single byte in the bytecode.
  lexer->interpolDepth++;

  Tok interpol = makeTok(lexer, TOK_INTERPOL);

  // skip the "#{"
  advance(lexer);
  advance(lexer);

  if (peek(lexer) == '}') {
    // i.e. empty interpolation
    advance(lexer);
    
    return makeTok(lexer, TOK_ERR);
  }

  return interpol;
}

static Tok str(Lexer *lexer) {
  while (peek(lexer) != '"' && !isAtEnd(lexer)) {
    if (peek(lexer) == '\n') {
      newline(lexer);
    }

    if (checkSeq(lexer, "#{")) {
      return interpol(lexer); 
    }

    advance(lexer);
  } 

  if (isAtEnd(lexer)) {
    return unexpectedChar(lexer);
  }

  advance(lexer);
  return makeTok(lexer, TOK_STR);
}

Tok nextTok(Lexer *lexer) {
  skipWs(lexer);
  sync(lexer);

  if (isAtEnd(lexer)) {
    return makeTok(lexer, TOK_EOF);
  }

  const char c = advance(lexer);

  if (isAlpha(c)) {
    return id(lexer);
  }

  if (isDigit(c)) {
    return number(lexer); 
  }

  switch (c) {
    case '(':
      return makeTok(lexer, TOK_LPAREN);

    case ')': 
      return makeTok(lexer, TOK_RPAREN);

    case '[': 
      return makeTok(lexer, TOK_LBRACKET);

    case ']': 
      return makeTok(lexer, TOK_RBRACKET);

    case '|':
      return makeTok(lexer, TOK_PIPE);

    case ',': 
      return makeTok(lexer, TOK_COMMA);

    case '-': 
      return makeTok(lexer, TOK_MINUS);

    case '+': 
      return makeTok(lexer, TOK_PLUS);

    case '/': 
      return makeTok(lexer, TOK_SLASH);

    case '*': 
      return makeTok(lexer, TOK_STAR); 

    case '&':
      return makeTok(lexer, TOK_BIT_AND);
    
    case '^':
      return makeTok(lexer, TOK_BIT_XOR);

    case '!':
      return makeTok(
        lexer,
        match(lexer, '=') ? TOK_NEQUAL : TOK_EXCLAM
      );

    case '=':
      return makeTok(
        lexer,
        match(lexer, '=') ? TOK_EQUAL : TOK_ASSIGN
      );

    case '>':
      return makeTok(
        lexer,
        match(lexer, '=') ? TOK_GREATER_EQUAL : 
        match(lexer, '>') ? TOK_SHR : TOK_GREATER
      );

    case '<':
      return makeTok(
        lexer,
        match(lexer, '=') ? TOK_LESS_EQUAL : 
        match(lexer, '<') ? TOK_SHL : TOK_LESS
      );

    case '.':
      return makeTok(
        lexer,
        match(lexer, '.') ? TOK_DOT_DOT : TOK_DOT
      );

    case ';':
    case '\n':
      newline(lexer);
      return makeTok(lexer, TOK_NEWLINE);

    case '"':
      return str(lexer);

    case '}':
      if (lexer->interpolDepth == 0) {
        return unexpectedChar(lexer);
      } 

      lexer->interpolDepth--;

      // discard the '}'
      sync(lexer);

      return str(lexer);

    default:
      return unexpectedChar(lexer);
  }
}
