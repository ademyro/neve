#include <stdarg.h>

#include "err.h"
#include "render.h"

ErrMod newErrMod(const char *fname, const char *src) {
  RenderCtx ctx = newRenderCtx(newLoc());

  ErrMod mod = {
    .src = src,
    .fname = fname,
    .ctx = ctx,
    .err = ERR_CLI
  };

  return mod;
}

void setNewErr(ErrMod *mod, Err id, Loc loc) {
  setErrLoc(mod, loc);
  setErr(mod, id);
  
  mod->errCount++;
}

void setErrLoc(ErrMod *mod, Loc loc) {
  mod->loc = loc;
  mod->ctx = newRenderCtx(loc);
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

  highlightErr(mod.ctx, fmt, args);

  va_end(args);
}

void showNote(ErrMod mod, Loc loc, const char *fmt, ...) {
  va_list args;

  mod.ctx.loc = loc;

  renderRegularLine(mod.ctx, mod.src);

  va_start(args, fmt); 

  highlightNote(mod.ctx, fmt, args);

  va_end(args);
}

void showHint(ErrMod mod, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  renderHint(mod.ctx, fmt, args);

  va_end(args);
}

void suggestFix(ErrMod mod, Loc fixLoc, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  renderModifiedLine(fixLoc, mod.src, fmt, args);
  highlightChange(fixLoc, fmt, args);

  va_end(args);
}

void suggestFixAbove(ErrMod mod, const char *fmt, ...) {
  va_list args;

  const Loc fixLoc = mod.loc;

  Loc lineBelow = newLoc();
  lineBelow.line = fixLoc.line + 1;

  va_start(args, fmt);

  renderFix(fixLoc, fmt, args);

  RenderCtx belowCtx = newRenderCtx(lineBelow);
  renderLine(belowCtx, mod.src);

  va_end(args);
}

void suggestExample(ErrMod mod, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  renderFmtLine(mod.ctx, fmt, args);
  va_end(args);
}

void endErr(ErrMod mod) {
  showHint(mod, "confused?  run " WHITE "`neve --whats E%03d`", mod.err);
}
