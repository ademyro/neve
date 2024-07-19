#include <stdio.h>

#include "debug.h"
#include "val.h"

static size_t simpleInstr(const char *name, size_t offset) {
  printf("%s\n", name);

  return offset + 1;
}

static size_t constInstr(const char *name, Chunk *ch, size_t offset) {
  const uint8_t constOffset = ch->code[offset + 1];

  printf("%-8s ", name);
  printVal(ch->consts.consts[constOffset]);
  printf(" (%u)\n", constOffset);

  return offset + 2;
}

static size_t longConstInstr(const char *name, Chunk *ch, size_t offset) {
  const uint8_t byteLength = 8;

  const uint32_t constOffset = (uint32_t)(
    ch->code[offset + 1] |
    (ch->code[offset + 2] << byteLength) |
    (ch->code[offset + 3] << byteLength * 2)
  );

  printf("%-8s ", name);
  printVal(ch->consts.consts[constOffset]);
  printf(" (%u)\n", constOffset);

  return offset + 4;
}

void disasmChunk(Chunk *ch, const char *name) {
  printf("%s:\n", name);
  size_t offset = 0;

  while (offset < ch->next) {
    offset = disasmInstr(ch, offset);
  }
}

size_t disasmInstr(Chunk *ch, size_t offset) {
  printf("%4zu  ", offset);

  const uint8_t instr = ch->code[offset];
  const int line = getLine(ch, offset);
  const int prevLine = offset > 0 ? getLine(ch, offset - 1) : -1;

  if (line == prevLine) {
    printf("   |  ");
  } else {
    printf("%4d  ", line);
  }

  switch (instr) {
    case OP_RET:
      return simpleInstr("ret", offset);
    
    case OP_CONST_LONG:
      return longConstInstr("pushl", ch, offset);
    
    case OP_CONST:
      return constInstr("push", ch, offset);
    
    default:
      printf("unknown instr %u\n", instr);
      return offset + 1;
  }

  return offset;
}
