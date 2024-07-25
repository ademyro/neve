#ifndef TOK_H
#define TOK_H

typedef struct {
  int line;
  int col;
  
  int length;
} Loc;

typedef enum {
  TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET, 
  TOK_COMMA, TOK_DOT, TOK_MINUS, TOK_PLUS, TOK_SEMI,
  TOK_SLASH, TOK_STAR,
  
  TOK_EXCLAM, TOK_NEQUAL, TOK_ASSIGN, TOK_EQUAL,
  TOK_GREATER, TOK_GREATER_EQUAL, TOK_LESS, TOK_LESS_EQUAL,

  TOK_ID, TOK_STR, TOK_INT, TOK_FLOAT,

  TOK_AND, TOK_CLASS, TOK_ELSE, TOK_END, 
  TOK_FALSE, TOK_FOR, TOK_FUN, TOK_IF, 
  TOK_LET, TOK_LOG, TOK_NIL, TOK_OR, 
  TOK_RETURN, TOK_SELF, TOK_TRUE, TOK_VAR, 
  TOK_WHILE, TOK_WITH,

  TOK_ERR, TOK_EOF
} TokType;

typedef struct {
  const TokType type; 

  const char *lexeme;
  const Loc loc;
} Tok;

Loc newLoc();
Loc mergeLocs(Loc left, Loc right);

Tok newTok(const char *lexeme, TokType type, Loc loc);

char *copyLexeme(Tok tok);

#endif
