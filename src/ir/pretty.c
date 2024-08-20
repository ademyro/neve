#include <stdarg.h>
#include <stdio.h>

#include "ir.h"
#include "pretty.h"
#include "tok.h"

static void write(const char *fmt, ...) {
  va_list args; 

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

static void newline(PrettyPrinter *printer) {
  write("\n%*s", printer->indentation, "");
}

static void indent(PrettyPrinter *printer) {
  printer->indentation += 2;
}

static void unindent(PrettyPrinter *printer) {
  printer->indentation -= 2;
}

static void printNode(PrettyPrinter *printer, Node *node);

static void printBinOp(PrettyPrinter *printer, BinOp binOp) {
  Tok op = binOp.op;

  write("( ");

  printNode(printer, binOp.left);
  write(" %.*s ", SHOW_LEXEME(op)); 
  printNode(printer, binOp.right);

  write(" )");
}

static void printUnOp(PrettyPrinter *printer, UnOp unOp) {
  Tok op = unOp.op;

  write("( ");

  write("%.*s ", SHOW_LEXEME(op));
  printNode(printer, unOp.operand); 

  write(" )");
}

static void printInt(Int i) {
  write("Int %ld", i.value);
}

static void printFloat(Float f) {
  write("Float %lf", f.value); 
}

static void printBool(Bool b) {
  write("Bool %s", b.value ? "true" : "false");
}

static void printStr(Str s) {
  Tok tok = s.str;
  
  write("Str %.*s", SHOW_LEXEME(tok));
}

static void printNode(PrettyPrinter *printer, Node *node) {
  switch (node->type) {
    case NODE_BINOP:
      printBinOp(printer, NODE_AS_BINOP(node));
      break;

    case NODE_UNOP:
      printUnOp(printer, NODE_AS_UNOP(node));
      break;

    case NODE_INT:
      printInt(NODE_AS_INT(node));
      break;

    case NODE_FLOAT:
      printFloat(NODE_AS_FLOAT(node));
      break;
    
    case NODE_BOOL:
      printBool(NODE_AS_BOOL(node));
      break;
    
    case NODE_NIL:
      write("Nil");
      break;

    case NODE_STR:
      printStr(NODE_AS_STR(node));

    default:
      break;
  }
}

void prettyPrint(Node *node) {
  IGNORE(newline);
  IGNORE(indent);
  IGNORE(unindent);

  PrettyPrinter printer = {
    .indentation = 0
  };

  printNode(&printer, node);

  newline(&printer);
}
