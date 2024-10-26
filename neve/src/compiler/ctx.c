#include "ctx.h"

Ctx newCtx(VM *vm, ErrMod mod, Chunk *ch) {
  Lexer lexer = newLexer(mod.src);
  Parser parser = newParser();
  TypeTable *table = allocTypeTable();

  Ctx ctx = {
    .vm = vm,
    .errMod = mod,
    .parser = parser,
    .lexer = lexer,
    .currCh = ch,
    .types = table
  };

  return ctx;
}
