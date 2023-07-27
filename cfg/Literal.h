#pragma once

#include "memory_manager/object/Object.h"

typedef enum {
  INTEGER_LITERAL,
  STRING_LITERAL,
} LiteralType;

typedef struct {
  LiteralType type;
  Object *obj;
} Literal;

Literal *Literal_Create_Integer();
Literal *Literal_Create_String();

Literal *Literal_Create_Integer_Init(Integer integer);
Literal *Literal_Create_String_Init(String string);

void Literal_Destroy(Literal *literal);

// CREATE 

Literal *__Literal_Create(LiteralType type, Object *obj) {
  assert(obj != NULL);
  Literal *literal = (Literal *) malloc(sizeof(Literal));
  literal->obj = obj;
  literal->type = type;
  return literal;
}

Literal *Literal_Create_Integer() {
  return __Literal_Create(INTEGER_LITERAL, Object_Integer_Create());
}

Literal *Literal_Create_String() {
  return __Literal_Create(STRING_LITERAL, Object_String_Create());
}

Literal *Literal_Create_Integer_Init(Integer integer) {
  return __Literal_Create(INTEGER_LITERAL, Object_Integer_Create_Init(integer));
}

Literal *Literal_Create_String_Init(String string) {
  return __Literal_Create(STRING_LITERAL, Object_String_Create_Init(string));
}

// DESTROY

void Literal_Destroy(Literal *literal) {
  assert(literal != NULL);
  
  Object_Destroy(literal->obj);
  free(literal);
}

// PRINT

void Literal_Print(Literal *literal, FILE *file) {
  assert(literal != NULL);
  
  Object_Print(literal->obj, file);
}