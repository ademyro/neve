#ifndef CTX_H
#define CTX_H

#include "compiler.h"
#include "err.h"
#include "lexer.h"

typedef struct {
  ErrMod errMod;
  Parser parser;
  Lexer lexer;
  Chunk *currCh;
} Ctx;

Ctx newCtx(ErrMod mod, Chunk *ch);

#endif
