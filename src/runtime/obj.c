#include <stdio.h>
#include <string.h>

#include "mem.h"
#include "obj.h"

#define ALLOC_OBJ(vm, type, objType) (type *)allocObj(vm, sizeof (type), objType)

static Obj *allocObj(VM *vm, size_t size, ObjType type) {
  Obj *obj = (Obj *)reallocate(NULL, 0, size);
  obj->type = type;

  obj->next = vm->objs;
  vm->objs = obj;

  return obj;
}

ObjStr *allocStr(VM *vm, bool ownsStr, const char *chars, size_t length) {
  ObjStr *str = ALLOC_OBJ(vm, ObjStr, OBJ_STR);
  str->ownsStr = ownsStr;
  str->length = length;
  str->chars = chars;

  return str;
}

void printObj(Val val) {
  switch (OBJ_TYPE(val)) {
    case OBJ_STR:
      printf("%.*s", (int)(VAL_AS_STR(val)->length), VAL_AS_CSTR(val));
      break;
  }
}

void freeObj(Obj *obj) {
  switch (obj->type) {
    case OBJ_STR: {
      ObjStr *str = (ObjStr *)obj;

      // the memory is actually allocated one address prior:
      // at the first double quote.  
      // "Hello, world!"
      // ^ here
      // however, because we trim the quotes in emit.c:emitStr(), 
      // "Hello, world!"   ->    Hello, world!
      //                 becomes
      // the allocated pointer lies one pointer ahead, so we need to offset
      // it by one to avoid a `free(): invalid pointer` error.
      char *allocatedPtr = (char *)(str->chars - 1);

      if (str->ownsStr) {
        FREE_ARR(char, allocatedPtr, str->length + 1);
      }

      FREE(ObjStr, obj);
      break;
    }
  }
}
