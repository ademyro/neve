#include <stdio.h>

#include "debug.h"
#include "val.h"

static size_t simpleInstr(const char *name, size_t offset) {
  printf("%-8s\n", name);

  return offset + 1;
}

static size_t constInstr(const char *name, Chunk *ch, size_t offset) {
  const uint8_t constOffset = ch->code[offset + 1];
  const uint8_t dest = ch->code[offset + 2];

  printf("%-8s ", name);
  printVal(ch->consts.consts[constOffset]);
  printf(" r%u (%u)\n", dest, constOffset);

  return offset + 3;
}

static size_t longConstInstr(const char *name, Chunk *ch, size_t offset) {
  const uint8_t byteLength = 8;
  const uint8_t instrLength = 5;

  const uint32_t constOffset = (uint32_t)(
    ch->code[offset + 1] |
    (ch->code[offset + 2] << byteLength) |
    (ch->code[offset + 3] << byteLength * 2)
  );

  const uint8_t dest = ch->code[offset + 4];

  printf("%-8s ", name);
  printVal(ch->consts.consts[constOffset]);
  printf(" r%u (%u)\n", dest, constOffset);

  return offset + instrLength;
}

static size_t regInstr(const char *name, Chunk *ch, Val *regs, size_t offset) {
  const uint8_t reg = ch->code[offset + 1];
  const Val val = regs[reg];

  printf("  r%u: ", reg);
  printVal(val);
  printf("\n");

  printf("%-8s r%u\n", name, reg);

  return offset + 2;
}

static size_t manyRegInstr(
  const char *name, 
  Chunk *ch, 
  Val *regs,
  size_t offset, 
  uint8_t regCount
) {
  printf("  ");

  // i'm sorry about this, but it's the easiest way to make this work
  for (uint8_t i = 0; i < regCount; i++) {
    uint8_t reg = ch->code[offset + 1];
    Val val = regs[reg];

    printf("r%u: ", reg);
    printVal(val);
    printf("    ");
  }

  printf("\n%-8s ", name);

  for (uint8_t i = 0; i < regCount; i++) {
    uint8_t reg = ch->code[offset + 1];

    printf("r%u ", reg);
  }

  printf("\n");

  return offset + regCount + 1;
}

static size_t byteInstr(const char *name, Chunk *ch, size_t offset) {
  const uint8_t opOffset = ch->code[offset + 1]; 
  
  printf("%-8s %u\n", name, opOffset);

  return offset + 2;
}

void disasmChunk(Chunk *ch, Val *regs, const char *name) {
  printf("%s:\n", name);
  size_t offset = 0;

  while (offset < ch->next) {
    offset = disasmInstr(ch, regs, offset);
  }
}

size_t disasmInstr(Chunk *ch, Val *regs, size_t offset) {
  IGNORE(byteInstr);

  printf("%4zu  ", offset);

  const uint8_t instr = ch->code[offset];

  switch (instr) {
    case OP_RET:
      return regInstr("ret", ch, regs, offset);

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
      return manyRegInstr("neg", ch, regs, offset, 2);

    case OP_NOT:
      return manyRegInstr("not", ch, regs, offset, 2);

    case OP_IS_NIL:
      return manyRegInstr("isnil", ch, regs, offset, 2);

    case OP_IS_ZERO:
      return manyRegInstr("isz", ch, regs, offset, 2);

    case OP_ADD:
      return manyRegInstr("add", ch, regs, offset, 3);

    case OP_SUB:
      return manyRegInstr("sub", ch, regs, offset, 3);

    case OP_MUL:
      return manyRegInstr("mul", ch, regs, offset, 3);

    case OP_DIV:
      return manyRegInstr("div", ch, regs, offset, 3);

    case OP_CONCAT:
      return manyRegInstr("concat", ch, regs, offset, 3);

    case OP_INTERPOL:
      return byteInstr("interpol", ch, offset);

    case OP_SHL:
      return manyRegInstr("shl", ch, regs, offset, 3);

    case OP_SHR:
      return manyRegInstr("shr", ch, regs, offset, 3);
    
    case OP_BIT_AND:
      return manyRegInstr("band", ch, regs, offset, 3);

    case OP_BIT_XOR:
      return manyRegInstr("xor", ch, regs, offset, 3);

    case OP_BIT_OR:
      return manyRegInstr("bor", ch, regs, offset, 3);

    case OP_EQ:
      return manyRegInstr("eq", ch, regs, offset, 3);

    case OP_NEQ:
      return manyRegInstr("neq", ch, regs, offset, 3);
    
    case OP_GREATER:
      return manyRegInstr("gt", ch, regs, offset, 3);

    case OP_LESS:
      return manyRegInstr("lt", ch, regs, offset, 3);

    case OP_GREATER_EQ:
      return manyRegInstr("gte", ch, regs, offset, 3);

    case OP_LESS_EQ:
      return manyRegInstr("lte", ch, regs, offset, 3);

    default:
      printf("unknown instr %u\n", instr);
      return offset + 1;
  }

  return offset;
}
