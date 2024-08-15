#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tok.h"

Loc newLoc() {
  Loc loc = { 
    .line = 1, 
    .col = 1, 
    .length = 0 
  };

  return loc;
}

Loc mergeLocs(Loc left, Loc right) {
  const int line = (left.line + right.line) / 2;     

  const int col = (int)fmin(left.col, right.col);

  const size_t length = (size_t)abs(left.col - right.col) + right.length;

  const Loc loc = { 
    .line = line, 
    .col = col,
    .length = length 
  };

  return loc;
}

void advanceLoc(Loc *loc) {
  loc->length++;
}

void newlineLoc(Loc *loc) {
  loc->line++;
  loc->col = 1;
  loc->length = 0;
}

void syncLoc(Loc *loc) {
  loc->col += (int)loc->length;
  loc->length = 0;
}

Tok newTok(const char *lexeme, const TokType type, Loc loc) {
  const Tok tok = { 
    .type = type, 
    .lexeme = lexeme, 
    .loc = loc 
  };

  return tok;
}

Tok emptyTok() {
  return newTok("", TOK_EOF, newLoc());
}

char *copyLexeme(Tok tok) {
  const size_t length = (size_t)tok.loc.length;

  char *lexeme = malloc(length + 1);
  memcpy(lexeme, tok.lexeme, length);
  lexeme[length] = '\0';

  return lexeme;
}
