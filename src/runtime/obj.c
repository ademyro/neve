#include <stdio.h>

#include "obj.h"

void printObj(Val val) {
  switch (OBJ_TYPE(val)) {
    case OBJ_STR:
      printf("%s", VAL_AS_CSTR(val));
      break;
  }
}
