#ifndef NEVE_PRETTY_H
#define NEVE_PRETTY_H

#include "ir.h"

typedef struct {
  int indentation;
} PrettyPrinter;

void prettyPrint(Node *node);

#endif
