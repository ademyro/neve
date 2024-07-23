#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "val.h"

#define STACK_MAX 256

typedef struct {
  Chunk *ch;
  uint8_t *ip;

  Val stack[STACK_MAX];
  Val *stackTop;
} VM;

typedef enum {
  AFTERMATH_OK,
  AFTERMATH_COMPILE_ERR,
  AFTERMATH_RUNTIME_ERR
} Aftermath;

VM newVM(Chunk *ch);
void freeVM(VM *vm);

Aftermath interpret(VM *vm);

void push(VM *vm, Val val);
Val pop(VM *vm);

#endif
