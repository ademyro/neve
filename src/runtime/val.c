#include <stdio.h>

#include "mem.h"
#include "val.h"

ValArr newValArr() {
  ValArr arr = {
    .cap = 0,
    .next = 0,
    .consts = NULL
  };

  return arr;
}

void writeValArr(ValArr *arr, Val val) {
  if (arr->next == arr->cap) {
    size_t oldCap = arr->cap; 

    arr->cap = GROW_CAP(oldCap);
    arr->consts = GROW_ARR(
      Val,
      arr->consts,
      oldCap,
      arr->cap
    );
  }

  arr->consts[arr->next++] = val;
}

void freeValArr(ValArr *arr) {
  FREE_ARR(Val, arr->consts, arr->cap);

  arr->cap = 0;
  arr->next = 0;
  arr->consts = NULL;
}

void printVal(Val val) {
  switch (val.type) {
    case VAL_BOOL:
      printf(VAL_AS_BOOL(val) ? "true" : "false");
      break;

    case VAL_NIL:
      printf("nil");
      break;
    
    case VAL_NUM:
      printf("%g", VAL_AS_NUM(val));
      break;
  }
}

bool valsEq(Val a, Val b) {
  if (a.type != b.type) {
    return false;
  }

  switch (a.type) {
    case VAL_NIL:
      return true;

    case VAL_BOOL:
      return VAL_AS_BOOL(a) && VAL_AS_BOOL(b);

    case VAL_NUM:
      return VAL_AS_NUM(a) == VAL_AS_NUM(b);
  }

  return false;
}
