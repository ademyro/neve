#ifndef RENDER_H
#define RENDER_H

#include <stdarg.h>

#include "tok.h"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

typedef struct {
  int lineDigits;
  Loc loc;
} RenderCtx;

RenderCtx newRenderCtx(Loc loc);

void renderErrMsg(int id, const char *fmt, va_list args);
void renderLocus(RenderCtx ctx, const char *fname);
void renderLine(RenderCtx ctx, const char *src);
void renderHint(RenderCtx ctx, const char *fmt, va_list args);
void renderFmtLine(RenderCtx ctx, const char *fmt, va_list args);

void highlightErr(RenderCtx ctx, const char *fmt, va_list args);
void highlightNote(RenderCtx ctx, const char *fmt, va_list args);

void renderModifiedLine(
  Loc fixLoc, 
  const char *src,
  const char *fmt, 
  va_list args
);

void renderFix(Loc fixLoc, const char *fmt, va_list args);
void highlightChange(Loc fixLoc, const char *fmt, va_list args);

#endif
