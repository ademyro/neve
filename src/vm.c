#include <stdio.h>

#include "common.h"
#include "vm.h"

#ifdef DEBUG_EXEC
#include "debug.h"
#endif

static void resetStack(VM *vm) {
  vm->stackTop = vm->stack;
}

VM newVM(Chunk *ch) {
  VM vm = {
    .ch = ch,
    .ip = ch->code,
    .stackTop = vm.stack
  };

  return vm;
}

void freeVM(VM *vm) {
  IGNORE(vm);

  // ...
}

static Aftermath run(VM *vm) {
#define READ_BYTE() (*vm->ip++)
#define READ_CONST() (vm->ch->consts.consts[READ_BYTE()])

  while (true) {
#ifdef DEBUG_EXEC
    const size_t offset = (size_t)(vm->ip - vm->ch->code);
    disasmInstr(vm->ch, offset);
#endif

    const uint8_t instr = READ_BYTE();

    switch (instr) {
      case OP_CONST: {
        Val val = READ_CONST();

        printVal(val);
        printf("\n");
        break;
      }

      case OP_RET:
        return AFTERMATH_OK;
      
      default:
        // TODO: add an error message
        return AFTERMATH_RUNTIME_ERR;
    }
  }

#undef READ_BYTE
#undef READ_CONST
}

Aftermath interpret(VM *vm) {
  return run(vm);
}
