#ifndef IR_H
#define IR_H

#include "tok.h"

#define NODE_AS_INT(node) ((node)->as.i)
#define NODE_AS_FLOAT(node) ((node)->as.f)
#define NODE_AS_UNOP(node) ((node)->as.unOp)
#define NODE_AS_BINOP(node) ((node)->as.binOp)

typedef enum {
  NODE_INT,
  NODE_FLOAT,
  NODE_UNOP,
  NODE_BINOP
} NodeType;

typedef struct Node Node;

typedef struct {
  long value;
  Loc loc;
} Int;

typedef struct {
  double value;
  Loc loc;
} Float;

typedef struct {
  Tok op;
  Node *operand;
} UnOp;

typedef struct {
  Node *left;
  Tok op;
  Node *right;
} BinOp;

struct Node {
  union {
    Int i;
    Float f;
    UnOp unOp;
    BinOp binOp;
  } as;

  NodeType type;
};

Node *newInt(long value, Loc loc);
Node *newFloat(double value, Loc loc);
Node *newUnOp(Tok op, Node *operand);
Node *newBinOp(Node *left, Tok op, Node *right);

void freeNode(Node *node);

#endif
