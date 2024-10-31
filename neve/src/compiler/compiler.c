#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "ctx.h"
#include "err.h"
#include "obj.h"

#ifdef DEBUG_COMPILE
// #include "debug.h"
#endif

#define UNEXPECTED_BYTE 0

static bool isBytecodeTruncated(Bytecode *bytecode) {
  const uint8_t *bytes = bytecode->bytes;
  const size_t length = bytecode->length;

  if (length < EOF_PADDING_SIZE) {
    return true;
  }

  return memcmp(
    bytes + length - EOF_PADDING_SIZE, 
    EOF_PADDING, 
    EOF_PADDING_SIZE
  ) != 0;
}

static bool checkMagicNumber(const uint8_t *bytes) {
  uint32_t firstFourBytes;
  memcpy(&firstFourBytes, bytes, sizeof (uint32_t));

  return firstFourBytes == NEVE_MAGIC_NUMBER;
}

static bool isValidBytecode(Bytecode *bytecode) {
  return !isBytecodeTruncated(bytecode) && checkMagicNumber(bytecode->bytes);
}

static size_t readObj(
  NeveVM *vm,
  ValArr *arr, 
  size_t offset, 
  Bytecode *bytecode
) {
  size_t newOffset = offset;

  const uint8_t *bytes = bytecode->bytes;

  uint8_t byte = bytes[newOffset++]; 
  ObjType type = (ObjType)byte;

  switch (type) {
    case OBJ_STR: {
      uint32_t length;
      memcpy(&length, bytes + newOffset, sizeof (uint32_t));

      newOffset += sizeof (uint32_t);

      const size_t strEndOffset = newOffset + (size_t)length;
      if (strEndOffset >= bytecode->length - EOF_PADDING_SIZE) {
        return UNEXPECTED_BYTE;
      }

      char *chars = (char *)(bytes + newOffset);

      newOffset += length;

      ObjStr *str = allocStr(vm, false, chars, length);
      writeValArr(arr, OBJ_VAL(str));

      break;
    }

    default:
      fprintf(stderr, "%d\n", type);
      return UNEXPECTED_BYTE;
  }

  return newOffset;
}

static size_t readConst(
  NeveVM *vm,
  ValArr *arr, 
  size_t offset, 
  Bytecode *bytecode
) {
  size_t newOffset = offset;

  const uint8_t *bytes = bytecode->bytes;

  uint8_t byte = bytes[newOffset++];
  ValType type = (ValType)byte;

  switch (type) {
    case VAL_NUM: {
      double n;
      memcpy(&n, bytes + newOffset, sizeof (double));

      newOffset += sizeof (double);

      writeValArr(arr, NUM_VAL(n));
      break;
    }

    case VAL_OBJ: {
      newOffset = readObj(vm, arr, newOffset, bytecode);

      if (newOffset == UNEXPECTED_BYTE) {
        return UNEXPECTED_BYTE;
      }

      break;
    }

    default:
      return UNEXPECTED_BYTE; 
  }

  return newOffset;
}

static bool readConsts(NeveVM *vm, ValArr *arr, Bytecode *bytecode) {
  const uint8_t *bytes = bytecode->bytes;
  size_t offset = sizeof (uint32_t);

  while (true) {
    uint8_t byte = bytes[offset];
    if (byte == NEVE_CONST_HEADER_SEPARATOR) {
      break;
    } 

    if (byte == EOF_PADDING_BYTE) {
      return false;
    }

    offset = readConst(vm, arr, offset, bytecode);
    if (offset == UNEXPECTED_BYTE) {
      return false;
    }
  }

  return true;
}
#undef UNEXPECTED_BYTE

bool compile(NeveVM *vm, const char *fname, Bytecode *bytecode, Chunk *ch) {
  ErrMod mod = newErrMod(fname);
  Ctx ctx = newCtx(vm, mod, ch);

  ErrMod newMod = ctx.errMod;

  IGNORE(newMod);

  if (!isValidBytecode(bytecode)) {
    cliErr("%s: unexpected file format", fname);
    cliErr("the bytecode file either contains invalid bytecode or");
    cliErr("it may have been truncated");

    return false;
  }

  ValArr consts = newValArr();

  if (!readConsts(vm, &consts, bytecode)) {
    cliErr("%s: failed to load constants", fname);
    
    freeValArr(&consts);
    return false;
  }

  for (size_t i = 0; i < consts.next; i++) {
    printVal(consts.consts[i]);
    printf("\n");
  }

  freeValArr(&consts);

  return true;
}
