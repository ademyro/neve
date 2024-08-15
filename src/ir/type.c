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

  TypeTable *table = malloc(sizeof (*table));
  table->intType = intType;
  table->floatType = floatType;
  table->boolType = boolType;
  table->nilType = nilType;

  return table;
}

bool getType(TypeTable *table, TypeKind kind, Type *t) {
  // we’ll definitely switch to a table
  switch (kind) {
    case TYPE_INT:
      *t = *table->intType;
      break;

    case TYPE_FLOAT:
      *t = *table->floatType;
      break;

    default:
      return false;
  }

  return true;
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

  table->intType->name = NULL;
  table->floatType->name = NULL;
  table->boolType->name = NULL;
  table->nilType->name = NULL;

  free(table->intType);
  free(table->floatType);
  free(table->boolType);
  free(table->nilType);

  table->intType = NULL;
  table->floatType = NULL;

  free(table);
  table = NULL;
}
