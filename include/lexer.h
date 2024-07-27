#ifndef LEXER_H
#define LEXER_H

#include "tok.h"

typedef struct {
  const char *start;
  const char *curr;

  Loc loc;

  int interpolDepth;
} Lexer;

Lexer newLexer(const char *src);
Tok nextTok(Lexer *lexer);

#endif
