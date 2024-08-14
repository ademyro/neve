#include "ctx.h"

Ctx newCtx(ErrMod mod, Chunk *ch) {
  Lexer lexer = newLexer(mod.src);
  Parser parser = newParser();
  TypeTable table = newTypeTable();

  Ctx ctx = {
    .errMod = mod,
    .parser = parser,
    .lexer = lexer,
    .currCh = ch,
    .types = &table
  };

  return ctx;
}
