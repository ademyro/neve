#include "opt.h"

static bool isConst(Node *node) {
  return (
    node->type == NODE_INT    ||
    node->type == NODE_FLOAT  ||
    node->type == NODE_BOOL   ||
    node->type == NODE_NIL
  );
}

static bool isFoldable(Node *node) {
  return (
    isConst(node)           ||
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

static bool getBoolValue(Node *node) {
  if (node->type == NODE_UNOP) {
    UnOp unOp = NODE_AS_UNOP(node);

    switch (unOp.op.type) {
      case TOK_NOT:
        return !getBoolValue(unOp.operand);

      default:
        return getBoolValue(unOp.operand);
    }
  }

  return NODE_AS_BOOL(node).value;
}

static void constFoldInt(Node *node) {
  BinOp *binOp = &NODE_AS_BINOP(node);

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
}

static void constFoldFloat(Node *node) {
  BinOp *binOp = &NODE_AS_BINOP(node);

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
}

static void constFoldBool(Node *node) {
  BinOp *binOp = &NODE_AS_BINOP(node);

  Tok op = binOp->op;

  Node *left = binOp->left;
  Node *right = binOp->right;

  if (op.type == TOK_EQUAL || op.type == TOK_NEQUAL) {
    if (!typesMatch(left->valType, right->valType)) {
      freeNode(left);
      freeNode(right);

      node->type = NODE_BOOL;

      Bool result = {
        .value = false,
        .loc = op.loc
      };

      node->as.b = result;
      return;
    }

    if (getTypeKind(left) == TYPE_NIL) {
      freeNode(left);
      freeNode(right);

      node->type = NODE_BOOL;

      Bool result = {
        .value = true,
        .loc = op.loc
      };

      node->as.b = result;
    }

    if (getTypeKind(left) == TYPE_BOOL) {
      const bool leftValue = getBoolValue(left);
      const bool rightValue = getBoolValue(right);

      const bool equal = rightValue && leftValue;

      freeNode(left);
      freeNode(right);

      node->type = NODE_BOOL;

      Bool result = {
        .value = (op.type == TOK_EQUAL) && equal,
        .loc = op.loc
      };

      node->as.b = result;
      return;
    }

    if (isNum(left)) {
      const double leftValue = getNumberValue(left);
      const double rightValue = getNumberValue(right);

      const bool equal = leftValue == rightValue;

      freeNode(left);
      freeNode(right);

      node->type = NODE_BOOL;

      Bool result = {
        .value = (op.type == TOK_EQUAL) && equal,
        .loc = op.loc
      };

      node->as.b = result;

      return;
    } 
  } else {
    const double leftValue = getNumberValue(left);
    const double rightValue = getNumberValue(right);

    bool result;

    switch (op.type) {
      case TOK_GREATER:
        result = leftValue > rightValue;     
        break;

      case TOK_LESS:
        result = leftValue < rightValue;
        break;
      
      case TOK_GREATER_EQUAL:
        result = leftValue >= rightValue;
        break;

      case TOK_LESS_EQUAL:
        result = leftValue <= rightValue;
        break;

      default:
        result = false;
        break;
    }

    freeNode(left);
    freeNode(right);

    node->type = NODE_BOOL;

    Bool resultNode = {
      .value = result,
      .loc = op.loc
    };

    node->as.b = resultNode;
    return;
  }
}

static void constFold(Node *node) {
  // the type is always known before optimization, so there’s no need
  // to call inferType().
  switch (getTypeKind(node)) {
    case TYPE_INT:
      constFoldInt(node);
      break;

    case TYPE_FLOAT:
      constFoldFloat(node);
      break;

    case TYPE_BOOL:
      constFoldBool(node);
      break;
      
    default:
      break;
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
    case NODE_BOOL:
    case NODE_NIL:
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

  if (isConst(unOp->operand)) {
    TypeKind kind = getTypeKind(unOp->operand);

    switch (kind) {
      case TYPE_BOOL:
      case TYPE_NIL: {
        // we can assume that the operand is `not` thanks to prior type
        // checking
        const bool operandValue = getBoolValue(unOp->operand);      

        freeNode(unOp->operand);

        node->type = NODE_BOOL;

        Bool negated = {
          .value = !operandValue,
          .loc = unOp->op.loc
        };

        node->as.b = negated;
        break;
      }

      case TYPE_INT: {
        // else, we can assume that the operator is `-`
        const long operandValue = (long)getNumberValue(unOp->operand);

        freeNode(unOp->operand);

        node->type = NODE_INT;

        Int simplified = {
          .value = -operandValue,
          .loc = unOp->op.loc
        };

        node->as.i = simplified;
        break;
      }

      case TYPE_FLOAT: {
        const double operandValue = getNumberValue(unOp->operand);

        freeNode(unOp->operand);

        node->type = NODE_FLOAT;

        Float simplified = {
          .value = -operandValue,
          .loc = unOp->op.loc
        };

        node->as.f = simplified;
        break;
      }

      default:
        break;
    }
  }
}
