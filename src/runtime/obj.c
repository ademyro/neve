#include <stdio.h>
#include <string.h>

#include "mem.h"
#include "obj.h"

#define ALLOC_OBJ(type, objType) (type *)allocObj(sizeof (type), objType)

static Obj *allocObj(size_t size, ObjType type) {
  Obj *obj = (Obj *)reallocate(NULL, 0, size);
  obj->type = type;

  return obj;
}

static ObjStr *allocStr(char *chars, size_t length) {
  ObjStr *str = ALLOC_OBJ(ObjStr, OBJ_STR);
  str->length = length;
  str->chars = chars;

  return str;
}

ObjStr *copyStr(const char *chars, size_t length) {
  char *heapChars = ALLOC(char, length + 1); 

  memcpy(heapChars, chars, length);

  heapChars[length] = '\0';

  return allocStr(heapChars, length);
}

void printObj(Val val) {
  switch (OBJ_TYPE(val)) {
    case OBJ_STR:
      printf("%s", VAL_AS_CSTR(val));
      break;
  }
}
