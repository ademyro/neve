#ifndef CTX_H
#define CTX_H

#include "compiler.h"
#include "err.h"
#include "lexer.h"
#include "type.h"
#include "vm.h"

typedef struct {
  VM *vm;
  ErrMod errMod;
  Parser parser;
  Lexer lexer;
  Chunk *currCh;
  TypeTable *types;
} Ctx;

Ctx newCtx(VM *vm, ErrMod mod, Chunk *ch);

#endif
