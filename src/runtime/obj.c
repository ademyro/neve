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

static ObjStr *allocStr(VM *vm, char *chars, size_t length) {
  ObjStr *str = ALLOC_OBJ(vm, ObjStr, OBJ_STR);
  str->length = length;
  str->chars = chars;

  return str;
}

ObjStr *takeStr(VM *vm, char *chars, size_t length) {
  return allocStr(vm, chars, length);
}

ObjStr *copyStr(VM *vm, const char *chars, size_t length) {
  char *heapChars = ALLOC(char, length + 1); 

  memcpy(heapChars, chars, length);

  heapChars[length] = '\0';

  return allocStr(vm, heapChars, length);
}

void printObj(Val val) {
  switch (OBJ_TYPE(val)) {
    case OBJ_STR:
      printf("%s", VAL_AS_CSTR(val));
      break;
  }
}

void freeObj(Obj *obj) {
  switch (obj->type) {
    case OBJ_STR: {
      ObjStr *str = (ObjStr *)obj;

      FREE_ARR(char, str->chars, str->length);
      FREE(ObjStr, obj);
      break;
    }
  }
}
