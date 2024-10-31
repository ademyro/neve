#ifndef NEVE_BYTECODE_H
#define NEVE_BYTECODE_H

#include "common.h"

typedef struct {
  const uint8_t *bytes;
  const size_t length;
} Bytecode;

Bytecode newBytecode(const uint8_t *bytes, size_t length);

#endif
