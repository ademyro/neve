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
  int line;
} RenderCtx;

void setLine(int line);
void renderErrMsg(const char *fmt, va_list args);
void renderLocus(const char *fname, Loc loc);
void renderLine(char *src, Loc loc);
void highlightErr(Loc loc, const char *fmt, va_list args);
void highlightNote(Loc loc, const char *fmt, va_list args);
void highlightChange(Loc loc, const char *fmt, va_list args);
void renderHint(const char *fmt, va_list args);
void renderModifiedLine(char *src, Loc loc, const char *fmt, va_list args);
void renderFix(Loc loc, const char *fmt, va_list args);
void renderFmtLine(Loc loc, const char *fmt, va_list args);

#endif
