#ifndef ERR_H
#define ERR_H

#include "tok.h"
#include "render.h"

typedef enum {
  ERR_CLI,
  ERR_SYNTAX,
  ERR_INVALID_VALUE,
  ERR_INFINITE_STR,
  ERR_INVALID_CHAR,
  ERR_INTEGER_OUT_OF_RANGE,
  ERR_INVALID_EXPR,
  ERR_OPEN_PARENS
} Err;

typedef struct {
  const char *fname;
  const char *src;

  Loc loc;
  RenderCtx ctx;

  Err err;

  int errCount;
} ErrMod;

ErrMod newErrMod(const char *fname, const char *src);
void setNewErr(ErrMod *mod, Err id, Loc loc);
void setErrLoc(ErrMod *mod, Loc loc);
void setErr(ErrMod *mod, Err id);

void cliErr(const char *fmt, ...);

void reportErr(ErrMod mod, const char *fmt, ...);

void showOffendingLine(ErrMod mod, const char *fmt, ...);
void showNote(ErrMod mod, const char *fmt, ...);
void showHint(ErrMod mod, const char *fmt, ...);

void suggestFix(ErrMod mod, Loc fixLoc, const char *fmt, ...);
void suggestFixAbove(ErrMod mod, const char *fmt, ...);
void suggestExample(ErrMod mod, const char *fmt, ...);

void endErr(ErrMod mod);

#endif
