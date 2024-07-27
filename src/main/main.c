#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "err.h"
#include "vm.h"

static const char *readFile(const char *filename) {
  FILE *f = fopen(filename, "rb");

  if (f == NULL) {
    cliErr("%s: file not found", filename);
    exit(1);
  }

  fseek(f, 0L, SEEK_END);
  size_t size = (size_t)ftell(f);
  rewind(f);

  char *buf = malloc(size + 1);

  if (buf == NULL) {
    cliErr("not enough memory available to read %s", filename);

    exit(1);
  }

  size_t end = fread(buf, sizeof (char), size, f);

  if (end < size) {
    cliErr("%s: couldn't read the full file", filename);
    cliErr("this is most likely because a call to fread() failed.");

    exit(1);
  }

  buf[end] = '\0';

  fclose(f);
  return buf;
}

static void repl() {
  // TODO: once we implement variable declarations, please implement
  // a better repl
  const size_t lim = 1024;
  char line[lim];

  while (true) {
    printf("? ");

    if (!fgets(line, (int)lim, stdin)) {
      printf("\n");
      break;
    }

    interpret(line);
  }
}

static void runFile(const char *filename) {
  const char *src = readFile(filename);

  Aftermath aftermath = interpret(src); 
  
  if (aftermath != AFTERMATH_OK) {
    exit(1);
  }
}

int main(const int argc, const char **argv) {
  IGNORE(argc);
  IGNORE(argv);

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    cliErr("usage: `neve [path]`");
    exit(1);
  }

  return 0;
}
