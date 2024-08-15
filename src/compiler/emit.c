#include "emit.h"
#include "chunk.h"

static void emitBinOp(Ctx *ctx, BinOp binOp) {
  emitNode(ctx, binOp.left);
  emitNode(ctx, binOp.right);

  const Tok op = binOp.op;

  // TODO: make this a table sometime and use it like this:
  // uint8_t opcode = binOpTable[op.type - TOK_PLUS];
  switch (op.type) {
    case TOK_PLUS:
      emit(ctx, OP_ADD, op.loc);
      break;

    case TOK_MINUS:
      emit(ctx, OP_SUB, op.loc);
      break;

    case TOK_STAR:
      emit(ctx, OP_MUL, op.loc);
      break;

    case TOK_SLASH:
      emit(ctx, OP_DIV, op.loc);
      break;

    case TOK_EQUAL:
      emit(ctx, OP_EQ, op.loc);
      break;

    case TOK_NEQUAL:
      emit(ctx, OP_NEQ, op.loc);
      break;

    case TOK_GREATER:
      emit(ctx, OP_GREATER, op.loc);
      break;

    case TOK_LESS:
      emit(ctx, OP_LESS, op.loc);
      break;

    case TOK_GREATER_EQUAL:
      emit(ctx, OP_GREATER_EQ, op.loc);
      break;

    case TOK_LESS_EQUAL:
      emit(ctx, OP_LESS_EQ, op.loc);
      break;

    default:
      // shouldn’t ever happen 
      break;
  }
}

static void emitUnOp(Ctx *ctx, UnOp unOp) {
  emitNode(ctx, unOp.operand); 

  const Tok op = unOp.op;

  switch (op.type) {
    case TOK_MINUS:
      emit(ctx, OP_NEG, op.loc);
      break;
    
    case TOK_NOT:
      emit(ctx, OP_NOT, op.loc);
      break;

    default:
      // will add more with time
      break;
  }
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

    case NODE_INT: {
      Int i = NODE_AS_INT(node);

      Val val = NUM_VAL((double)i.value);
      emitConst(ctx, val, i.loc);
      break;
    }
    
    case NODE_FLOAT: {
      Float f = NODE_AS_FLOAT(node);
      
      Val val = NUM_VAL(f.value);
      emitConst(ctx, val, f.loc);
      break;
    }
    
    case NODE_BOOL: {
      Bool b = NODE_AS_BOOL(node);

      Val val = BOOL_VAL(b.value);
      emitConst(ctx, val, b.loc);
      break;
    }

    case NODE_NIL: {
      Loc loc = NODE_AS_NIL(node);
      
      emitConst(ctx, NIL_VAL, loc);
      break;
    }

  }
}