#include "ctx.h"

Ctx newCtx(NeveVM *vm, ErrMod mod, Chunk *ch) {
  Ctx ctx = {
    .vm = vm,
    .errMod = mod,
    .currCh = ch
  };

  return ctx;
}
