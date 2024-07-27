#include <stdio.h>

#include "compiler.h"
#include "lexer.h"

void compile(const char *src) {
  Lexer lexer = newLexer(src); 

  int line = -1;
  while (true) {
    Tok tok = nextTok(&lexer);

    if (tok.loc.line != line) {
      printf("%4d ", tok.loc.line);
      line = tok.loc.line;
    } else {
      printf("   | ");
    }

    if (tok.type == TOK_NEWLINE) {
      printf("%2d '\\n'\n", tok.type);
    } else {
      printf("%2d '%.*s'\n", tok.type, (int)tok.loc.length, tok.lexeme); 
    }

    if (tok.type == TOK_EOF) {
      break;
    }
  }
}

