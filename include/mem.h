#ifndef MEM_H
#define MEM_H

#include "common.h"

#define ALLOC(type, size)                                   \
  (type *)reallocate(NULL, 0, sizeof (type) * (size))

#define GROW_CAP(cap) ((cap) < 8 ? 8 : (cap) * 2)
#define GROW_ARR(type, ptr, oldSize, newSize)               \
  (type *)reallocate(                                       \
    ptr,                                                    \
    sizeof (type) * (oldSize),                              \
    sizeof (type) * (newSize)                               \
  )

#define FREE_ARR(type, ptr, oldSize)                        \
  reallocate(ptr, sizeof (type) * (oldSize), 0)

void *reallocate(void *ptr, size_t oldSize, size_t newSize);

#endif
