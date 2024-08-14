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

static Type inferUnOp(TypeTable *table, UnOp node) {
  return inferType(table, node.operand);
}

static Type inferBinOp(TypeTable *table, BinOp node) {
  const Type leftType = inferType(table, node.left);
  const Type rightType = inferType(table, node.right);

  switch (node.op.type) {
    case TOK_PLUS:
    case TOK_MINUS:
    case TOK_STAR:
      /* 
      for TOK_PLUS:
      if (leftType.kind == TYPE_STR || rightType.kind == TYPE_STR)
      */
      if (typesMatch(leftType, rightType)) {
        return leftType;
      }

      if (leftType.kind == TYPE_FLOAT || rightType.kind == TYPE_FLOAT) {
        return table->floatType;
      } 

      break;

    case TOK_SLASH:
      return table->floatType;

    default:
      break;
  }

  return unknownType();
}

Node *newInt(TypeTable *table, long value, Loc loc) {
  Int i = {
    .value = value,
    .loc = loc
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_INT;
  node->valType = table->intType;

  node->as.i = i;

  return node;
}

Node *newFloat(TypeTable *table, double value, Loc loc) {
  Float f = {
    .value = value,
    .loc = loc
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_FLOAT;
  node->valType = table->intType;

  node->as.f = f;

  return node;
}

Node *newUnOp(TypeTable *table, Tok op, Node *operand) {
  UnOp unOp = {
    .op = op,
    .operand = operand
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_UNOP;
  node->valType = inferUnOp(table, unOp);

  node->as.unOp = unOp;

  return node;
}

Node *newBinOp(TypeTable *table, Node *left, Tok op, Node *right) {
  BinOp binOp = {
    .left = left,
    .op = op,
    .right = right
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_BINOP;
  node->valType = inferBinOp(table, binOp);

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

Type inferType(TypeTable *table, Node *node) {
  if (isTypeKnown(node->valType)) {
    return node->valType;
  }

  switch (node->type) {
    case NODE_INT:
      return table->intType; 

    case NODE_FLOAT:
      return table->floatType;

    case NODE_UNOP:
      return inferUnOp(table, NODE_AS_UNOP(node));

    case NODE_BINOP:
      return inferBinOp(table, NODE_AS_BINOP(node));

    default:
      return unknownType();
  }
}
