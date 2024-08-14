#ifndef CTX_H
#define CTX_H

#include "compiler.h"
#include "err.h"
#include "lexer.h"
#include "type.h"

typedef struct {
  ErrMod errMod;
  Parser parser;
  Lexer lexer;
  Chunk *currCh;
  TypeTable *types;
} Ctx;

Ctx newCtx(ErrMod mod, Chunk *ch);

#endif
