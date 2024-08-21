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

static void freeBool(Bool *node) {
  node->value = false;
  node->loc = newLoc();
}

static void freeStr(Str *node) {
  if (node->ownsLexeme) {
    free((char *)node->str.lexeme);
  }

  node->str = emptyTok();
  node->ownsLexeme = false;
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
  Tok op = node.op;

  if (op.type == TOK_NOT) {
    return *table->boolType;
  }

  return inferType(table, node.operand);
}

static Type inferBinOp(TypeTable *table, BinOp node) {
  const Type leftType = inferType(table, node.left);
  const Type rightType = inferType(table, node.right);

  switch (node.op.type) {
    case TOK_PLUS:
      if (leftType.kind == TYPE_STR && rightType.kind == TYPE_STR) {
        return *table->strType; 
      }

      __attribute__ ((fallthrough));
      // else: fallthrough
      // |
      // v
    case TOK_MINUS:
    case TOK_STAR:
      if (typesMatch(leftType, rightType)) {
        return leftType;
      }

      if (leftType.kind == TYPE_FLOAT || rightType.kind == TYPE_FLOAT) {
        return *table->floatType;
      } 

      break;

    case TOK_SLASH:
      return *table->floatType;

    case TOK_SHL:
    case TOK_SHR:
    case TOK_BIT_AND:
    case TOK_BIT_XOR:
    case TOK_PIPE:
      return *table->intType;

    default:
      return *table->boolType;
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
  node->valType = *table->intType;

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
  node->valType = unknownType();
  node->valType = *table->floatType;

  node->as.f = f;

  return node;
}

Node *newBool(TypeTable *table, bool value, Loc loc) {
  Bool b = {
    .value = value,
    .loc = loc
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_BOOL;
  node->valType = unknownType();
  node->valType = *table->boolType;

  node->as.b = b;

  return node;
}

Node *newNil(TypeTable *table, Loc loc) {
  Node *node = malloc(sizeof (*node));
  node->type = NODE_NIL;
  node->valType = unknownType();
  node->valType = *table->nilType;

  node->as.nilLoc = loc;

  return node;
}

Node *newStr(TypeTable *table, Tok tok) {
  Str str = {
    .str = tok,
    .ownsLexeme = false
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_STR;
  node->valType = unknownType();
  node->valType = *table->strType;

  node->as.str = str;

  return node;
}

Node *newUnOp(TypeTable *table, Tok op, UnOpType opType, Node *operand) {
  UnOp unOp = {
    .op = op,
    .opType = opType,
    .operand = operand
  };

  Node *node = malloc(sizeof (*node));
  node->type = NODE_UNOP;
  node->valType = unknownType();
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
  node->valType = unknownType();
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

    case NODE_BOOL:
      freeBool(&NODE_AS_BOOL(node));
      break;
    
    case NODE_NIL:
      node->as.nilLoc = newLoc();
      break;

    case NODE_STR:
      freeStr(&NODE_AS_STR(node));
      break;
    
    case NODE_UNOP:
      freeUnOp(&NODE_AS_UNOP(node));
      break;

    case NODE_BINOP:
      freeBinOp(&NODE_AS_BINOP(node));
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
      return *table->intType; 

    case NODE_FLOAT:
      return *table->floatType;

    case NODE_BOOL:
      return *table->boolType;

    case NODE_NIL:
      return *table->nilType;

    case NODE_UNOP:
      return inferUnOp(table, NODE_AS_UNOP(node));

    case NODE_BINOP:
      return inferBinOp(table, NODE_AS_BINOP(node));

    default:
      return unknownType();
  }
}

bool checkType(Node *node, TypeKind kind) {
  return node->valType.kind == kind;
}

bool isNum(Node *node) {
  return checkType(node, TYPE_FLOAT) || checkType(node, TYPE_INT);
}

Loc getLoc(Node *node) {
  switch (node->type) {
    case NODE_BINOP:
      return NODE_AS_BINOP(node).op.loc;

    case NODE_UNOP:
      return NODE_AS_UNOP(node).op.loc;

    case NODE_INT:
      return NODE_AS_INT(node).loc;

    case NODE_FLOAT:
      return NODE_AS_FLOAT(node).loc;

    case NODE_BOOL:
      return NODE_AS_BOOL(node).loc;

    case NODE_NIL:
      return NODE_AS_NIL(node);

    case NODE_STR:
      return NODE_AS_STR(node).str.loc;
  }

  return newLoc();
}

Loc getFullLoc(Node *node) {
  switch (node->type) {
    case NODE_BINOP: {
      BinOp binOp = NODE_AS_BINOP(node);
      return mergeLocs(
        mergeLocs(getFullLoc(binOp.left), binOp.op.loc),
        getFullLoc(binOp.right)
      );
    }

    case NODE_UNOP: {
      UnOp unOp = NODE_AS_UNOP(node);

      return mergeLocs(unOp.op.loc, getFullLoc(unOp.operand));
    }
    
    default:
      return getLoc(node);
  }
}
