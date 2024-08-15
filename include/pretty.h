#ifndef PRETTY_H
#define PRETTY_H

#include "ir.h"

typedef struct {
  int indentation;
} PrettyPrinter;

void prettyPrint(Node *node);

#endif
