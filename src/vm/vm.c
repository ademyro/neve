#include <stdio.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "mem.h"
#include "obj.h"
#include "vm.h"

#ifdef DEBUG_EXEC
#include "debug.h"
#endif

#ifdef DEBUG_EXEC
static void printStack(VM *vm) {
  printf("    ");

  for (Val *v = vm->stack; v < vm->stackTop; v++) {
    printf("[");
    printVal(*v);
    printf("] ");
  } 

  printf("\n");
}
#endif

VM newVM() {
  VM vm = {
    .objs = NULL
  };

  return vm;
}

void freeVM(VM *vm) {
  freeObjs(vm->objs);
  vm->objs = NULL;
}

void resetStack(VM *vm) {
  vm->stackTop = vm->stack;
}

static Aftermath run(VM *vm) {
#define READ_BYTE() (*vm->ip++)
#define READ_CONST() (vm->ch->consts.consts[READ_BYTE()])
#define BIN_OP(valType, op)                                     \
  do {                                                          \
    double b = VAL_AS_NUM(pop(vm));                             \
    double a = VAL_AS_NUM(pop(vm));                             \
                                                                \
    push(vm, valType(a op b));                                  \
  } while (false)
#define BIT_OP(op)                                              \
  do {                                                          \
    int b = (int)VAL_AS_NUM(pop(vm));                           \
    int a = (int)VAL_AS_NUM(pop(vm));                           \
                                                                \
    push(vm, NUM_VAL(a op b));                                  \
  } while (false)

  while (true) {
#ifdef DEBUG_EXEC
    printStack(vm);

    const size_t offset = (size_t)(vm->ip - vm->ch->code);
    disasmInstr(vm->ch, offset);
#endif

    const uint8_t instr = READ_BYTE();

    switch (instr) {
      case OP_CONST: {
        const Val val = READ_CONST();

        push(vm, val);
        break;
      }
      
      case OP_CONST_LONG: {
        Chunk *ch = vm->ch;

        const uint8_t byteLength = 8;
        const uint32_t constOffset = (uint32_t)(
          ch->code[offset + 1] |
          (ch->code[offset + 2] << byteLength) |
          (ch->code[offset + 3] << byteLength * 2)
        );

        const Val val = ch->consts.consts[constOffset]; 

        push(vm, val);
        break;
      }

      case OP_TRUE:
        push(vm, BOOL_VAL(true));
        break;

      case OP_FALSE:
        push(vm, BOOL_VAL(false));
        break;

      case OP_NIL:
        push(vm, NIL_VAL);
        break;

      case OP_ZERO:
        push(vm, NUM_VAL(0));
        break;

      case OP_ONE:
        push(vm, NUM_VAL(1));
        break;

      case OP_MINUS_ONE:
        push(vm, NUM_VAL(-1));
        break;

      case OP_NEG:
        vm->stackTop[-1] = NUM_VAL(-VAL_AS_NUM(vm->stackTop[-1]));
        break;

      case OP_NOT:
        vm->stackTop[-1] = BOOL_VAL(!VAL_AS_BOOL(vm->stackTop[-1]));
        break;

      case OP_IS_NIL:
        vm->stackTop[-1] = BOOL_VAL(!IS_VAL_NIL(vm->stackTop[-1]));
        break;

      case OP_IS_ZERO:
        vm->stackTop[-1] = BOOL_VAL(VAL_AS_NUM(vm->stackTop[-1]) == 0);
        break;

      case OP_IS_MINUS_ONE:
        vm->stackTop[-1] = BOOL_VAL(VAL_AS_NUM(vm->stackTop[-1]) == -1);
        break;

      case OP_ADD:
        BIN_OP(NUM_VAL, +);
        break;

      case OP_SUB:
        BIN_OP(NUM_VAL, -);
        break;

      case OP_MUL:
        BIN_OP(NUM_VAL, *);
        break;

      case OP_DIV:
        BIN_OP(NUM_VAL, /);
        break;

      case OP_CONCAT: {
        ObjStr *b = VAL_AS_STR(pop(vm));
        ObjStr *a = VAL_AS_STR(pop(vm));

        size_t length = a->length + b->length;

        char *chars = ALLOC(char, length + 1);

        memcpy(chars, a->chars, a->length);
        memcpy(chars + a->length, b->chars, b->length);

        chars[length] = '\0';

        ObjStr *result = allocStr(vm, true, chars, length);
        push(vm, OBJ_VAL(result));
        break;
      }

      case OP_SHL:
        BIT_OP(<<);
        break;

      case OP_SHR:
        BIT_OP(>>);
        break;

      case OP_BIT_AND:
        BIT_OP(&);
        break;

      case OP_BIT_XOR:
        BIT_OP(^);
        break;

      case OP_BIT_OR:
        BIT_OP(|);
        break;

      case OP_EQ: {
        Val b = pop(vm);
        Val a = pop(vm);

        push(vm, BOOL_VAL(valsEq(a, b)));
        break;
      }

      case OP_NEQ: {
        Val b = pop(vm);
        Val a = pop(vm);

        push(vm, BOOL_VAL(!valsEq(a, b)));
        break;
      }

      case OP_GREATER:
        BIN_OP(BOOL_VAL, >);
        break;

      case OP_LESS:
        BIN_OP(BOOL_VAL, <);
        break;

      case OP_GREATER_EQ:
        BIN_OP(BOOL_VAL, >=);
        break;

      case OP_LESS_EQ:
        BIN_OP(BOOL_VAL, <=);
        break;

      case OP_RET:
        printVal(pop(vm));
        printf("\n");
        return AFTERMATH_OK;
      
      default:
        // TODO: add an error message
        return AFTERMATH_RUNTIME_ERR;
    }
  }

#undef READ_BYTE
#undef READ_CONST
#undef BIN_OP
#undef BIT_OP
}

Aftermath interpret(const char *fname, VM *vm, const char *src) {
  Chunk ch = newChunk();

  if (!compile(vm, fname, src, &ch)) {
    freeChunk(&ch); 

    return AFTERMATH_COMPILE_ERR;
  }

  vm->ch = &ch;
  vm->ip = ch.code;

  Aftermath aftermath = run(vm);

  freeChunk(&ch);

  return aftermath;
}

void push(VM *vm, Val val) {
  *vm->stackTop = val;

  vm->stackTop++;
}

Val pop(VM *vm) {
  vm->stackTop--;

  return *vm->stackTop;
}
