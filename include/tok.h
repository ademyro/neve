#ifndef TOK_H
#define TOK_H

#include "common.h"

#define SHOW_LEXEME(tok) (int)((tok).loc.length), ((tok).lexeme)

typedef struct {
  int line;
  int col;
  
  size_t length;
} Loc;

typedef enum {
  TOK_COMMA, TOK_DOT, TOK_DOT_DOT, TOK_SLASH, 
  TOK_STAR,

  TOK_NEQUAL, TOK_ASSIGN, TOK_EQUAL, TOK_GREATER, 
  TOK_GREATER_EQUAL, TOK_LESS, TOK_LESS_EQUAL,

  TOK_SHL, TOK_SHR, TOK_BIT_AND, TOK_BIT_XOR,

  TOK_EXCLAM, TOK_QUESTION,

  TOK_AND, TOK_CLASS, TOK_ELSE, TOK_END, 
  TOK_ENUM, TOK_FOR, TOK_FUN, TOK_IF, 
  TOK_LET, TOK_LOG, TOK_MATCH, TOK_OR, 
  TOK_RETURN, TOK_VAR, TOK_WHILE,

  TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET, 
  TOK_PIPE, TOK_MINUS, TOK_PLUS,

  TOK_ID, TOK_STR, TOK_INT, TOK_FLOAT,
  TOK_INTERPOL,

  TOK_DO, TOK_FALSE, TOK_NIL, TOK_NOT, TOK_SELF, 
  TOK_TRUE, TOK_WITH,

  TOK_NEWLINE, TOK_ERR, TOK_EOF
} TokType;

typedef struct {
  TokType type; 

  const char *lexeme;
  Loc loc;
} Tok;

Loc newLoc();
Loc mergeLocs(Loc left, Loc right);

void advanceLoc(Loc *loc);
void newlineLoc(Loc *loc);
void syncLoc(Loc *loc);

Tok newTok(const char *lexeme, TokType type, Loc loc);
Tok emptyTok();

char *copyLexeme(Tok tok);

void trimStrTokQuotes(Tok *tok);

#endif
