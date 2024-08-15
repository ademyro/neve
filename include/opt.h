#ifndef OPT_H
#define OPT_H

#include "ir.h"

void optNode(Node *node);
void optBinOp(Node *node);
void optUnOp(Node *node);

#endif
