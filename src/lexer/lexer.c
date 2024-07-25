#include "lexer.h"

Lexer newLexer(const char *src) {
  Lexer lexer = {
    .start = src,
    .curr = src,
    .loc = newLoc()
  };

  return lexer;
}
