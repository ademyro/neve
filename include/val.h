#ifndef VAL_H
#define VAL_H

#include "common.h"

typedef double Val;

typedef struct {
  size_t cap; 
  size_t next;

  Val *consts;
} ValArr;

ValArr newValArr();
void writeValArr(ValArr *arr, Val val);
void freeValArr(ValArr *arr);
void printVal(Val val);

#endif
