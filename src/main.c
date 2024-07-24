#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv) {
  IGNORE(argc);
  IGNORE(argv);

  Chunk ch = newChunk();

  const Val onePointTwo = 1.2;
  const Val threePointFour = 3.4;
  writeConst(&ch, onePointTwo, 1);
  writeConst(&ch, threePointFour, 1);

  writeChunk(&ch, OP_ADD, 2);

  const Val fivePointSix = 5.6;
  writeConst(&ch, fivePointSix, 2);
  writeChunk(&ch, OP_DIV, 2);

  writeChunk(&ch, OP_NEG, 3);

  const Val zero = 0; 
  writeConst(&ch, zero, 4);

  writeChunk(&ch, OP_DIV, 4);
  writeChunk(&ch, OP_RET, 4);
  disasmChunk(&ch, "test");

  VM vm = newVM(&ch);

  resetStack(&vm);
  interpret(&vm);

  freeVM(&vm);
  freeChunk(&ch);

  return 0;
}
