#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, char **argv) {
  IGNORE(argc);
  IGNORE(argv);

  Chunk ch = newChunk();

  const int loopEnd = 260;
  for (int i = 0; i < loopEnd; i++) {
    writeConst(&ch, (Val)i, 1);
  }

  writeChunk(&ch, OP_RET, 2);
  disasmChunk(&ch, "test");

  freeChunk(&ch);

  return 0;
}
