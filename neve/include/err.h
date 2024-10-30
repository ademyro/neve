#ifndef ERR_H
#define ERR_H

#include "render.h"

typedef enum {
  ERR_CLI,
  ERR_LIST_OUT_OF_BOUNDS
} Err;

typedef struct {
  const char *fname;
  const char *src;

  RenderCtx ctx;
  int line;

  Err err;

  int errCount;
} ErrMod;

ErrMod newErrMod(const char *fname);
void setNewErr(ErrMod *mod, Err id, int line);
void setErrLoc(ErrMod *mod, int line);
void setErr(ErrMod *mod, Err id);

void cliErr(const char *fmt, ...);

void reportErr(ErrMod mod, const char *fmt, ...);

void showOffendingLine(ErrMod mod, const char *fmt, ...);
void showNote(ErrMod mod, int line, const char *fmt, ...);
void showHint(ErrMod mod, const char *fmt, ...);

#endif
