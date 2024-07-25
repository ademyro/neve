#include "common.h"
#include "err.h"

/*
static void repl() {
  // TODO: implement once the error module is complete
}

static void runFile(const char *filename) {
  // TODO: implement once the error module is complete
}
*/

int main(const int argc, const char **argv) {
  IGNORE(argc);
  IGNORE(argv);

  ErrMod mod = newErrMod(
    "test.neve", 
    "let my_str = @not an expr\n"
    "didn't expect that"
  );

  const int col = 14;
  const int length = 12;

  Loc loc = newLoc();
  loc.col = col;
  loc.length = length;
  loc.line = 1;

  setErrLoc(&mod, loc);

  reportErr(mod, "unexpected token");
  showOffendingLine(mod, "was looking for a value (like an int)");
  showHint(mod, "you might’ve made a typo or forgotten a comma."); 
  showHint(mod, "here's an example of a well-formed expression:");
  suggestExample(mod, "    let x = y + z - 42");
  showHint(mod, "where:");
  suggestExample(mod, "    y + z - 42");
  showHint(mod, "is the well formed expression.");

  return 0;
}
