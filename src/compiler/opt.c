#include "opt.h"

static bool isFoldable(Node *node) {
  return (
    node->type == NODE_INT    ||
    node->type == NODE_FLOAT  ||
    node->type == NODE_UNOP
  );
}

static bool isUnOp(Node *node) {
  return node->type == NODE_UNOP;
}

static TypeKind getTypeKind(Node *node) {
  return node->valType.kind;
}

static double getUnOpValue(Node *node);
static double getNumberValue(Node *node);

static double getUnOpValue(Node *node) {
  UnOp unOp = NODE_AS_UNOP(node);

  switch (unOp.op.type) {
    case TOK_MINUS:
      return -getNumberValue(unOp.operand);

    default:
      return getNumberValue(unOp.operand);
  }
}

static double getNumberValue(Node *node) {
  if (node->type == NODE_UNOP) {
    return getUnOpValue(node);
  }

  if (getTypeKind(node) == TYPE_FLOAT) {
    return NODE_AS_FLOAT(node).value;
  }

  return (double)NODE_AS_INT(node).value;
}

static void constFold(Node *node) {
  BinOp *binOp = &NODE_AS_BINOP(node);

  // the type is always known before optimization, so there’s no need
  // to call inferType().
  if (getTypeKind(node) == TYPE_INT) {
    long leftValue = (long)getNumberValue(binOp->left);
    long rightValue = (long)getNumberValue(binOp->right);

    Loc loc = binOp->op.loc;
    TokType op = binOp->op.type;

    long result;
    switch (op) {
      case TOK_PLUS:
        result = leftValue + rightValue;
        break;

      case TOK_MINUS:
        result = leftValue - rightValue;
        break;

      case TOK_STAR:
        result = leftValue * rightValue;
        break;

      case TOK_SLASH:
        result = leftValue / rightValue;
        break;

      default:
        // this shouldn’t ever happen; but the compiler complains
        // if there’s no default case
        result = -1L;
        break;
    }

    freeNode(binOp->left);
    freeNode(binOp->right);
    
    node->type = NODE_INT;

    Int resultNode = {
      .value = result,
      .loc = loc
    };

    node->as.i = resultNode;
    return;
  }

  if (getTypeKind(node) == TYPE_FLOAT) {
    double leftValue = getNumberValue(binOp->left);
    double rightValue = getNumberValue(binOp->right);

    Loc loc = binOp->op.loc;
    TokType op = binOp->op.type;

    double result;
    switch (op) {
      case TOK_PLUS:
        result = leftValue + rightValue;
        break;

      case TOK_MINUS:
        result = leftValue - rightValue;
        break;

      case TOK_STAR:
        result = leftValue * rightValue;
        break;

      case TOK_SLASH:
        result = leftValue / rightValue;
        break;

      default:
        // this shouldn’t ever happen; but the compiler complains
        // if there’s no default case
        result = -1L;
        break;
    }

    freeNode(binOp->left);
    freeNode(binOp->right);

    node->type = NODE_FLOAT;
    
    Float resultNode = {
      .value = result,
      .loc = loc
    };

    node->as.f = resultNode;
    return;
  }
}

void optNode(Node *node) {
  switch (node->type) {
    case NODE_BINOP:
      optBinOp(node);
      break;

    case NODE_UNOP:
      optUnOp(node);
      break;

    case NODE_INT:
    case NODE_FLOAT:
      break;
  }
}

void optBinOp(Node *node) {
  // TODO: implement unary canonicalization
  BinOp *binOp = &NODE_AS_BINOP(node);

  optNode(binOp->left);
  optNode(binOp->right);

  if (isFoldable(binOp->left) && isFoldable(binOp->right)) {
    constFold(node);
  } 
}

void optUnOp(Node *node) {
  UnOp *unOp = &NODE_AS_UNOP(node);

  Node *nodeOperand = unOp->operand;
  UnOp *operand = &NODE_AS_UNOP(unOp->operand);

  optNode(unOp->operand);

  if (isUnOp(unOp->operand) && operand->op.type == unOp->op.type) {
    node->type = operand->operand->type;
    node->as = operand->operand->as;

    freeNode(nodeOperand);
  }
}
