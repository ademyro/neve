#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "vm.h"

static const uint8_t *readFile(const char *fname) {
  FILE *f = fopen(fname, "rb");

  if (f == NULL) {
    cliErr("%s: file not found", fname);
    exit(1);
  }

  fseek(f, 0L, SEEK_END);
  size_t size = (size_t)ftell(f);
  rewind(f);

  uint8_t *buf = malloc(size);

  if (buf == NULL) {
    cliErr("not enough memory available to read %s", fname);

    exit(1);
  }

  size_t end = fread(buf, sizeof (uint8_t), size, f);

  if (end < size) {
    cliErr("%s: couldn't read the full file", fname);
    cliErr("this is most likely because a call to fread() failed.");

    exit(1);
  }

  fclose(f);
  return buf;
}

static void runFile(const char *fname) {
  NeveVM vm = newVM();
  resetStack(&vm);

  const uint8_t *bytes = readFile(fname);

  // Aftermath aftermath = interpret(fname, &vm, bytes); 

  freeVM(&vm);
  free((uint8_t *)bytes);

  /*
  if (aftermath != AFTERMATH_OK) {
    exit(1);
  }
  */
}

int main(const int argc, const char **argv) {
  if (argc != 2) {
    cliErr("usage: `neve <path>`");
    exit(1);
  }

  runFile(argv[1]);

  return 0;
}
