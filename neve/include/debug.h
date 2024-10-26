#ifndef DEBUG_H
#define DEBUG_H

#include "chunk.h"

void disasmChunk(Chunk *ch, const char *name);
size_t disasmInstr(Chunk *ch, size_t offset);

#endif
