#pragma once

#include "./basic_objects/Integer.h"
#include "ObjectHeader.h"

// DEFINITIONS

typedef struct {
  Integer integer;
} IntegerObjectData;

Object *Object_Integer_Create();
Object *Object_Integer_Create_Init(Integer integer);
void Object_Integer_Destroy(Object *obj);
void Object_Integer_Print(Object *obj, FILE *file);

// IMPLEMENTATIONS

Object *Object_Integer_Create() {
  typed_data(IntegerObjectData);
  Integer i = Integer_Create(0);
  data->integer = i;
  return Object_Create(INTEGER_OBJ, INTEGER_SIZE, data, type_create_generic_int_type());
}

Object *Object_Integer_Create_Init(Integer integer) {
  typed_data(IntegerObjectData);
  data->integer = integer;
  return Object_Create(INTEGER_OBJ, INTEGER_SIZE, data, type_create_generic_int_type());
}

void Object_Integer_Destroy(Object *obj) {
  assert(Object_Is_Integer(obj));
  casted_data(IntegerObjectData, obj);
  free(data);
}

void Object_Integer_Print(Object *obj, FILE *file) {
  assert(Object_Is_Integer(obj));

  casted_data(IntegerObjectData, obj);
  Integer_Print(data->integer, file);
}