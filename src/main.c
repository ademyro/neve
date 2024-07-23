#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv) {
  IGNORE(argc);
  IGNORE(argv);

  Chunk ch = newChunk();
  // do some magic

  const Val onePointTwo = 1.2;
  writeConst(&ch, onePointTwo, 1);

  writeChunk(&ch, OP_RET, 2);
  disasmChunk(&ch, "test");

  VM vm = newVM(&ch);

  interpret(&vm);

  freeVM(&vm);
  freeChunk(&ch);

  return 0;
}
