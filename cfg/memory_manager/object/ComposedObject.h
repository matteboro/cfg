#pragma once

#include "Object.h"
#include "../../../prsr/strct_decl.h"

typedef struct {
  StructDeclaration *struct_decl;  // can be NULL if it was an array
  size_t size;
  ObjectArray *objects;
} ComposedObjectData;

Object *Object_Composed_Create(
  ByteSize byte_size, 
  size_t size, 
  ObjectArray *objects, 
  StructDeclaration *struct_decl,
  Type *real_type);
void Object_Composed_Destroy(Object *obj);
void Object_Composed_Print(Object *obj, FILE *file);

// IMPLEMENTATION

Object *Object_Composed_Create(
  ByteSize byte_size, 
  size_t size, 
  ObjectArray *objects, 
  StructDeclaration *struct_decl,
  Type *real_type) {

  assert(objects != NULL);

  typed_data(ComposedObjectData);
  data->size = size;
  data->struct_decl = struct_decl;
  data->objects = objects;
  return Object_Create(COMPOSED_OBJ, byte_size, data, real_type);
}

void Object_Composed_Destroy(Object *obj) {
  assert(obj != NULL);
  assert(Object_Is_Composed(obj));

  casted_data(ComposedObjectData, obj);
  ObjectArray_Destroy(data->objects);
  free(data);
}

void Object_Composed_Print(Object *obj, FILE *file) {
  assert(obj != NULL);
  assert(Object_Is_Composed(obj));

  casted_data(ComposedObjectData, obj);
  ObjectArray_Print(data->objects, file);
}
