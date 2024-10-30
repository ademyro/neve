#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include "vm.h"

#define CHECK_PANIC(ctx)                                    \
  do {                                                      \
    if ((ctx)->parser.isPanicking) {                        \
      return;                                               \
    }                                                       \
                                                            \
    (ctx)->parser.isPanicking = true;                       \
  } while (false)

#define IS_PANICKING(ctx) ((ctx)->parser.isPanicking)

bool compile(NeveVM *vm, const char *fname, const uint8_t *bytes, Chunk *ch);

#endif
