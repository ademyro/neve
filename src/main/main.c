#include <stdio.h>

#include "common.h"

static void repl() {
  // TODO: implement once the error module is complete
}

static void runFile(const char *filename) {
  // TODO: implement once the error module is complete
}

int main(const int argc, const char **argv) {
  IGNORE(argc);
  IGNORE(argv);

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    // TODO: replace with error
    fprintf(stderr, "Usage: neve [path]\n");
  }

  return 0;
}
