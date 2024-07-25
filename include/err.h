#ifndef nevec_error_h
#define nevec_error_h

#include "tok.h"

typedef struct {
    const char *fname;
    char *src;
} ErrMod;

void initErrMod(const char *fname, char *src);
void cliErr(const char *fmt, ...);
void reportErrAt(Loc loc, const char *fmt, ...);
void showOffendingLine(Loc loc, const char *fmt, ...);
void showNote(Loc loc, const char *fmt, ...);
void showHint(const char *fmt, ...);
void suggestFix(Loc loc, const char *fmt, ...);
void suggestFixAbove(int line, const char *fmt, ...);
void suggestExample(Loc loc, const char *fmt, ...);

#endif
