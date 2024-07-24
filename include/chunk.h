#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "val.h"

typedef enum {
  OP_CONST,
  OP_CONST_LONG,
  OP_NEG,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_RET
} OpCode;

typedef struct {
  size_t offset;
  int line;
} Line;

typedef struct {
  size_t cap;
  size_t next;

  Line *lines;
} LineArr;

typedef struct {
  size_t cap;
  size_t next;

  uint8_t *code;
  ValArr consts;

  LineArr lines;
} Chunk;

Chunk newChunk();
void writeChunk(Chunk *ch, uint8_t byte, int line);
void freeChunk(Chunk *ch);
void writeConst(Chunk *ch, Val val, int line);
int addConst(Chunk *ch, Val val);

LineArr newLineArr();
void writeLineArr(LineArr *arr, int line, size_t offset);
void freeLineArr(LineArr *arr);

int getLine(Chunk *ch, size_t offset);

#endif
