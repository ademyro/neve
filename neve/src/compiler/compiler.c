#include <limits.h>

#include "compiler.h"
#include "ctx.h"
#include "err.h"

#ifdef DEBUG_COMPILE
// #include "debug.h"
#endif

bool compile(NeveVM *vm, const char *fname, const uint8_t *bytes, Chunk *ch) {
  IGNORE(bytes);

  ErrMod mod = newErrMod(fname);
  Ctx ctx = newCtx(vm, mod, ch);

  ErrMod newMod = ctx.errMod;

  const bool hadErrs = newMod.errCount != 0;

  if (!hadErrs) {
    
  }

  if (hadErrs) {
    cliErr("compilation failed due to %d previous errors", newMod.errCount);
  }

  return !hadErrs;
}
