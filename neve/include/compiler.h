#ifndef COMPILER_H
#define COMPILER_H

#include "tok.h"
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

typedef struct {
  Tok curr;
  Tok prev;

  bool isPanicking;
} Parser;

Parser newParser();

bool compile(VM *vm, const char *fname, const char *src, Chunk *ch);

#endif
