#ifndef VAL_H
#define VAL_H

#include "common.h"

#define BOOL_VAL(val) ((Val){ VAL_BOOL, {.boolean = (val) } })
#define NIL_VAL       ((Val){ VAL_NIL, { .num = 0 } })
#define NUM_VAL(val)  ((Val){ VAL_NUM, { .num = (val) } })

#define IS_VAL_BOOL(val)  ((val).type == VAL_BOOL)
#define IS_VAL_NIL(val)   ((val).type == VAL_NIL)
#define IS_VAL_NUM(val)   ((val).type == VAL_NUM)

#define VAL_AS_BOOL(val)    ((val).as.boolean)
#define VAL_AS_NUM(val)     ((val).as.num)

typedef enum {
  VAL_NUM,
  VAL_BOOL,
  VAL_NIL
} ValType;

typedef struct {
  ValType type;

  union {
    bool boolean;
    double num;
  } as;
} Val;

typedef struct {
  size_t cap; 
  size_t next;

  Val *consts;
} ValArr;

ValArr newValArr();
void writeValArr(ValArr *arr, Val val);
void freeValArr(ValArr *arr);
void printVal(Val val);

bool valsEq(Val a, Val b);

#endif
