#include <stdio.h>

#include "compiler.h"
#include "lexer.h"

void compile(const char *src) {
  Lexer lexer = newLexer(src); 

  int line = -1;
  for (;;) {
    Tok tok = nextTok(&lexer);

    if (tok.loc.line != line) {
      printf("%4d ", tok.loc.line);
      line = tok.loc.line;
    } else {
      printf("   | ");
    }

    printf("%2d '%.*s'\n", tok.type, tok.loc.length, tok.lexeme); 

    if (tok.type == TOK_EOF) {
      break;
    }
  }
}

