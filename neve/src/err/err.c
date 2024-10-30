#include <stdarg.h>

#include "err.h"
#include "render.h"

ErrMod newErrMod(const char *fname) {
  RenderCtx ctx = newRenderCtx(1);

  ErrMod mod = {
    // .src = src,
    .fname = fname,
    .ctx = ctx,
    .err = ERR_CLI
  };

  return mod;
}

void setNewErr(ErrMod *mod, Err id, int line) {
  setErrLoc(mod, line);
  setErr(mod, id);
  
  mod->errCount++;
}

void setErrLoc(ErrMod *mod, int line) {
  mod->line = line;
  mod->ctx = newRenderCtx(line);
}

void setErr(ErrMod *mod, Err id) {
  mod->err = id; 
}

void cliErr(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  renderErrMsg(ERR_CLI, fmt, args);
  va_end(args);
}

void reportErr(ErrMod mod, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  renderErrMsg(mod.err, fmt, args); 
  renderLocus(mod.ctx, mod.fname); 

  va_end(args);
}

void showOffendingLine(ErrMod mod, const char *fmt, ...) {
  va_list args;

  renderLine(mod.ctx, mod.src);

  va_start(args, fmt);

  va_end(args);
}

void showNote(ErrMod mod, int line, const char *fmt, ...) {
  va_list args;

  mod.ctx.line = line;

  renderRegularLine(mod.ctx, mod.src);

  va_start(args, fmt); 

  va_end(args);
}

void showHint(ErrMod mod, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  renderHint(mod.ctx, fmt, args);

  va_end(args);
}
