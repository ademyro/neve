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

static size_t byteInstr(const char *name, Chunk *ch, size_t offset) {
  const uint8_t opOffset = ch->code[offset + 1]; 
  
  printf("%-8s %u\n", name, opOffset);

  return offset + 2;
}

void disasmChunk(Chunk *ch, const char *name) {
  printf("%s:\n", name);
  size_t offset = 0;

  while (offset < ch->next) {
    offset = disasmInstr(ch, offset);
  }
}

size_t disasmInstr(Chunk *ch, size_t offset) {
  IGNORE(byteInstr);

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

    case OP_TRUE:
      return simpleInstr("true", offset);

    case OP_FALSE:
      return simpleInstr("false", offset);

    case OP_NIL:
      return simpleInstr("nil", offset);

    case OP_ZERO:
      return simpleInstr("pushz", offset);

    case OP_MINUS_ONE:
      return simpleInstr("pushm1", offset);

    case OP_ONE:
      return simpleInstr("push1", offset);
    
    case OP_NEG:
      return simpleInstr("neg", offset);

    case OP_NOT:
      return simpleInstr("not", offset);

    case OP_IS_NIL:
      return simpleInstr("isnil", offset);

    case OP_IS_ZERO:
      return simpleInstr("isz", offset);
    
    case OP_IS_MINUS_ONE:
      return simpleInstr("ism1", offset);

    case OP_ADD:
      return simpleInstr("add", offset);

    case OP_SUB:
      return simpleInstr("sub", offset);

    case OP_MUL:
      return simpleInstr("mul", offset);

    case OP_DIV:
      return simpleInstr("div", offset);

    case OP_CONCAT:
      return simpleInstr("concat", offset);

    /*
    case OP_INTERPOL:
      return byteInstr("interpol", ch, offset);
    */

    case OP_SHL:
      return simpleInstr("shl", offset);

    case OP_SHR:
      return simpleInstr("shr", offset);
    
    case OP_BIT_AND:
      return simpleInstr("band", offset);

    case OP_BIT_XOR:
      return simpleInstr("xor", offset);

    case OP_BIT_OR:
      return simpleInstr("bor", offset);

    case OP_EQ:
      return simpleInstr("eq", offset);

    case OP_NEQ:
      return simpleInstr("neq", offset);
    
    case OP_GREATER:
      return simpleInstr("gt", offset);

    case OP_LESS:
      return simpleInstr("lt", offset);

    case OP_GREATER_EQ:
      return simpleInstr("gte", offset);

    case OP_LESS_EQ:
      return simpleInstr("lte", offset);

    default:
      printf("unknown instr %u\n", instr);
      return offset + 1;
  }

  return offset;
}
