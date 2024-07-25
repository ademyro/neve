#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "render.h"

static int digitsIn(int n) {
  return (int)floor(log10(abs(n)) + 1);
}

static void write(const char *s) {
  fprintf(stderr, "%s", s);
}

static void writef(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

static void endFormat() {
  write(RESET "\n");
}

static const char *findLine(const char *src, int line) {
  if (src == NULL || line < 1) {
    return NULL;
  }

  int currLine = 1; 
  const char *curr = src;

  while (*curr != '\0' && currLine < line) {
    if (*curr == '\n') {
      currLine++;
    }

    curr++;
  }

  if (currLine < line) {
    return NULL;
  }

  return curr;
}

static void writeFrom(const char *src, const int until) {
  int col = 1;
  const char *curr = src;

  while (col <= until) {
    fputc(*curr, stderr); 
    curr++;
    col++;
  }
}

static void writeEmptyPipe(const int lineDigits) {
  writef(BLUE "%*s | \n", lineDigits, "");
}

static void writeLinePipes(const int lineDigits, const int line) {
  writeEmptyPipe(lineDigits);
  writef("%*d | " RESET, lineDigits, line); 
}

static void highlight(int length, char highlighter) {
  for (int i = 0; i < length; i++) {
    fputc(highlighter, stderr);
  } 

  fputc(' ', stderr);
}

RenderCtx newRenderCtx(Loc loc) {
  RenderCtx ctx = {
    .loc = loc,
    .lineDigits = digitsIn(loc.line + 1)
  };

  return ctx;
}

void renderErrMsg(int id, const char *fmt, va_list args) {
  write(RED "error" WHITE ": ");

  vfprintf(stderr, fmt, args);

  if (id < 1) {
    endFormat();
    return;
  }

  writef(RED " [E%03d]", id);

  endFormat();
}

void renderLocus(RenderCtx ctx, const char *fname) {
  write(BLUE "   in" WHITE ": ");

  writef("%s:%d:%d", fname, ctx.loc.line, ctx.loc.col);

  endFormat();
}

void renderLine(RenderCtx ctx, const char *src) {
  Loc loc = ctx.loc;

  const int line = loc.line;
  const char *lineStart = findLine(src, line);
  const int lineEnd = (int)strcspn(lineStart, "\n");

  writeLinePipes(ctx.lineDigits, loc.line);

  int col = loc.col - 1;
  int errEnd = col + loc.length;

  if (lineStart == NULL) {
    write(RED "could not find line");
  }

  writeFrom(lineStart, col);
  write(RED);
  writeFrom(lineStart + col, loc.length);
  write(RESET);
  writeFrom(lineStart + errEnd, lineEnd - errEnd);

  endFormat();
}

void highlightErr(RenderCtx ctx, const char *fmt, va_list args) {
  writef(BLUE "%*s |", ctx.lineDigits, "");
  writef("%*s" RED, ctx.loc.col, "");

  highlight(ctx.loc.length, '^');

  vfprintf(stderr, fmt, args);

  endFormat();
}

void highlightNote(RenderCtx ctx, const char *fmt, va_list args) {
  writef(BLUE "%*s |", ctx.lineDigits, "");
  writef("%*s", ctx.loc.col, "");

  highlight(ctx.loc.length, '-');

  vfprintf(stderr, fmt, args);

  endFormat();
}

void highlightChange(Loc fixLoc, const char *fmt, va_list args) {
  int changeLength = vsnprintf(NULL, 0, fmt, args);

  const int newLineDigits = digitsIn(fixLoc.col);
  writef(BLUE "%*s |", newLineDigits, ""); 
  writef("%*s" GREEN, fixLoc.col, "");

  highlight(changeLength, '+');

  endFormat();
}

void renderHint(RenderCtx ctx, const char *fmt, va_list args) {
  writef(BLUE "%*s -> ", ctx.lineDigits, "");

  vfprintf(stderr, fmt, args);

  endFormat();
}

void renderModifiedLine(
  Loc fixLoc, 
  const char *src, 
  const char *fmt, 
  va_list args
) {
  int line = fixLoc.line;
  const char *lineStart = findLine(src, line);
  int lineEnd = (int)strcspn(lineStart, "\n\0");

  const int newLineDigits = digitsIn(line);

  writeLinePipes(newLineDigits, line);

  int col = fixLoc.col;

  writeFrom(lineStart, col);
  write(GREEN);
  vfprintf(stderr, fmt, args);
  write(RESET);
  writeFrom(lineStart + col, lineEnd);

  endFormat();
}

void renderFix(Loc fixLoc, const char *fmt, va_list args) {
  const int newLineDigits = digitsIn(fixLoc.line);

  writeLinePipes(newLineDigits, fixLoc.line);

  write(GREEN);
  vfprintf(stderr, fmt, args);
  highlightChange(fixLoc, fmt, args);

  endFormat();
}

void renderFmtLine(RenderCtx ctx, const char *fmt, va_list args) {
  writeLinePipes(ctx.lineDigits, ctx.loc.line);

  vfprintf(stderr, fmt, args);

  endFormat();
  writeEmptyPipe(ctx.lineDigits);
}
