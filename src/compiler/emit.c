#include "chunk.h"
#include "emit.h"
#include "obj.h"

static uint8_t binOpTable[] = {
  OP_ADD, OP_SUB, OP_MUL, OP_DIV,
  OP_SHL, OP_SHR, OP_BIT_AND, OP_BIT_XOR,
  OP_BIT_OR, OP_EQ, OP_NEQ, OP_GREATER, 
  OP_LESS, OP_GREATER_EQ, OP_LESS_EQ
};

static void emitBinOp(Ctx *ctx, BinOp binOp) {
  emitNode(ctx, binOp.left);
  emitNode(ctx, binOp.right);

  const Tok op = binOp.op;

  uint8_t opcode = binOpTable[op.type - TOK_PLUS];

  if (
    opcode == OP_ADD &&
    checkType(binOp.left, TYPE_STR) && 
    checkType(binOp.right, TYPE_STR)
  ) {
    emit(ctx, OP_CONCAT, op.loc); 
    return;
  }

  emit(ctx, opcode, op.loc);
}

static void emitUnOp(Ctx *ctx, UnOp unOp) {
  emitNode(ctx, unOp.operand); 

  Loc loc = unOp.op.loc;
  const UnOpType op = unOp.opType;

  switch (op) {
    case UNOP_NEG:
      emit(ctx, OP_NEG, loc);
      break;
    
    case UNOP_NOT:
      emit(ctx, OP_NOT, loc);
      break;

    default:
      // yeah this is pretty silly
      if (op & UNOP_IS_NIL) {
        emit(ctx, OP_IS_NIL, loc);
      }

      if (op & UNOP_IS_ZERO) {
        emit(ctx, OP_IS_ZERO, loc);
      }

      if (op & UNOP_IS_NEG_ONE) {
        emit(ctx, OP_IS_MINUS_ONE, loc);
      }

      if (op & UNOP_NEG) {
        emit(ctx, OP_NEG, loc);
      }
      break;
  }
}

static void emitInt(Ctx *ctx, Int node) {
  switch (node.value) {
    case -1L:
      emit(ctx, OP_MINUS_ONE, node.loc);
      break;

    case 0L:
      emit(ctx, OP_ZERO, node.loc);
      break;

    case 1L:
      emit(ctx, OP_ONE, node.loc);
      break;

    default:
      emitConst(ctx, NUM_VAL((double)node.value), node.loc);
      break;
  }
}

static void emitFloat(Ctx *ctx, Float node) {
  if (node.value == -1) {
    emit(ctx, OP_MINUS_ONE, node.loc);
    return;
  }

  if (node.value == 0) {
    emit(ctx, OP_ZERO, node.loc);
    return;
  }

  if (node.value == 1) {
    emit(ctx, OP_ONE, node.loc);
    return;
  }

  emitConst(ctx, NUM_VAL(node.value), node.loc);
}

static void emitStr(Ctx *ctx, Str node) {
  Tok tok = node.str;

  const char *trimmedLexeme = tok.lexeme + 1;
  const size_t trimmedLength = tok.loc.length - 2;

  Val val = OBJ_VAL(copyStr(ctx->vm, trimmedLexeme, trimmedLength));

  emitConst(ctx, val, tok.loc);
}

Chunk *currChunk(Ctx *ctx) {
  return ctx->currCh;
}

void emit(Ctx *ctx, uint8_t byte, Loc loc) {
  writeChunk(currChunk(ctx), byte, loc.line);
}

void emitConst(Ctx *ctx, Val val, Loc loc) {
  writeConst(currChunk(ctx), val, loc.line);
}

void emitBoth(Ctx *ctx, uint8_t one, uint8_t two, Loc loc) {
  emit(ctx, one, loc);
  emit(ctx, two, loc);
}

void emitReturn(Ctx *ctx, Loc loc) {
  emit(ctx, OP_RET, loc);
}

void emitNode(Ctx *ctx, Node *node) {
  switch (node->type) {
    case NODE_BINOP:
      emitBinOp(ctx, NODE_AS_BINOP(node));
      break;

    case NODE_UNOP:
      emitUnOp(ctx, NODE_AS_UNOP(node));
      break;

    case NODE_INT:
      emitInt(ctx, NODE_AS_INT(node));
      break;
    
    case NODE_FLOAT:
      emitFloat(ctx, NODE_AS_FLOAT(node));
      break;
    
    case NODE_BOOL: {
      Bool b = NODE_AS_BOOL(node);

      emit(ctx, b.value ? OP_TRUE : OP_FALSE, b.loc);
      break;
    }

    case NODE_NIL: {
      Loc loc = NODE_AS_NIL(node);
      
      emitConst(ctx, NIL_VAL, loc);
      break;
    }

    case NODE_STR: {
      emitStr(ctx, NODE_AS_STR(node));
      break;
    }
  }
}
