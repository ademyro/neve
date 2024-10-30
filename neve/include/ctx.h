#ifndef CTX_H
#define CTX_H

#include "err.h"
#include "vm.h"

typedef struct {
  NeveVM *vm;
  ErrMod errMod;
  Chunk *currCh;
} Ctx;

Ctx newCtx(NeveVM *vm, ErrMod mod, Chunk *ch);

#endif
