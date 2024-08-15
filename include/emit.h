#ifndef EMIT_H
#define EMIT_H

#include "ctx.h"
#include "ir.h"

Chunk *currChunk(Ctx *ctx);

void emit(Ctx *ctx, uint8_t byte, Loc loc);
void emitBoth(Ctx *ctx, uint8_t one, uint8_t two, Loc loc);
void emitConst(Ctx *ctx, Val val, Loc loc);
void emitReturn(Ctx *ctx, Loc loc);

void emitNode(Ctx *ctx, Node *node);

#endif
