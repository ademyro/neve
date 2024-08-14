#include "ctx.h"

Ctx newCtx(ErrMod mod, Chunk *ch) {
  Lexer lexer = newLexer(mod.src);
  Parser parser = newParser();

  Ctx ctx = {
    .errMod = mod,
    .parser = parser,
    .lexer = lexer,
    .currCh = ch
  };

  return ctx;
}
