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
  printf("%g", val);
}
