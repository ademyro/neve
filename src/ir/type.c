#include "type.h"

TypeTable newTypeTable() {
  Type intType = {
    .kind = TYPE_INT,
    .name = "Int"
  };

  Type floatType = {
    .kind = TYPE_FLOAT,
    .name = "Float"
  };

  TypeTable table = {
    .intType = intType,
    .floatType = floatType
  };

  return table;
}

bool getType(TypeTable *table, TypeKind kind, Type *t) {
  // we’ll definitely switch to a table
  switch (kind) {
    case TYPE_INT:
      *t = table->intType;
      break;

    case TYPE_FLOAT:
      *t = table->floatType;
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
