#include <stdlib.h>

#include "type.h"

TypeTable *allocTypeTable() {
  Type *intType = malloc(sizeof (*intType));
  intType->kind = TYPE_INT;
  intType->name = "Int";

  Type *floatType = malloc(sizeof (*floatType));
  floatType->kind = TYPE_FLOAT;
  floatType->name = "Float";

  Type *boolType = malloc(sizeof (*boolType));
  boolType->kind = TYPE_BOOL;
  boolType->name = "Bool";

  Type *nilType = malloc(sizeof (*nilType));
  nilType->kind = TYPE_NIL;
  nilType->name = "Nil";

  Type *strType = malloc(sizeof (*strType));
  strType->kind = TYPE_STR;
  strType->name = "Str";

  TypeTable *table = malloc(sizeof (*table));
  table->intType = intType;
  table->floatType = floatType;
  table->boolType = boolType;
  table->nilType = nilType;
  table->strType = strType;

  return table;
}

bool typesMatch(Type a, Type b) {
  return a.kind == b.kind;
}

bool isTypeKnown(Type t) {
  return t.kind != TYPE_UNKNOWN;
}

Type unknownType() {
  Type pending = {
    .kind = TYPE_UNKNOWN
  };

  return pending;
}

void freeTypeTable(TypeTable *table) {
  table->intType->kind = TYPE_UNKNOWN;
  table->floatType->kind = TYPE_UNKNOWN;
  table->boolType->kind = TYPE_UNKNOWN;
  table->nilType->kind = TYPE_UNKNOWN;
  table->strType->kind = TYPE_UNKNOWN;

  table->intType->name = NULL;
  table->floatType->name = NULL;
  table->boolType->name = NULL;
  table->nilType->name = NULL;
  table->strType->name = NULL;

  free(table->intType);
  free(table->floatType);
  free(table->boolType);
  free(table->nilType);
  free(table->strType);

  table->intType = NULL;
  table->floatType = NULL;
  table->boolType = NULL;
  table->nilType = NULL;
  table->strType = NULL;

  free(table);
  table = NULL;
}
