#include <stdlib.h>

#include "ir.h"

static void freeInt(Int *node) {
  node->value = 0;
  node->loc = newLoc();
}

static void freeFloat(Float *node) {
  node->value = 0;
  node->loc = newLoc();
}

static void freeUnOp(UnOp *node) {
  node->op = emptyTok();
  freeNode(node->operand);
}

static void freeBinOp(BinOp *node) {
  freeNode(node->left);
  node->op = emptyTok();
  freeNode(node->right);
}

Node *newInt(long value, Loc loc) {
  Int i = {
    .value = value,
    .loc = loc
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_INT;
  node->as.i = i;

  return node;
}

Node *newFloat(double value, Loc loc) {
  Float f = {
    .value = value,
    .loc = loc
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_FLOAT;
  node->as.f = f;

  return node;
}

Node *newUnOp(Tok op, Node *operand) {
  UnOp unOp = {
    .op = op,
    .operand = operand
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_UNOP;
  node->as.unOp = unOp;

  return node;
}

Node *newBinOp(Node *left, Tok op, Node *right) {
  BinOp binOp = {
    .left = left,
    .op = op,
    .right = right
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_BINOP;
  node->as.binOp = binOp;

  return node;
}

void freeNode(Node *node) {
  switch (node->type) {
    case NODE_INT:
      freeInt(&NODE_AS_INT(node));
      break;
    
    case NODE_FLOAT:
      freeFloat(&NODE_AS_FLOAT(node));
      break;
    
    case NODE_UNOP:
      freeUnOp(&NODE_AS_UNOP(node));
      break;

    case NODE_BINOP:
      freeBinOp(&NODE_AS_BINOP(node));
      break;
    
    default:
      break;
  }

  free(node);
  node = NULL;
}
