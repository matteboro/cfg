#pragma once

#include "ObjectHeader.h"

// DEFINITION

typedef struct {
  MemTableIndex index;
  Type *pointed_type;
} PointerObjectData;

// IMPLEMENTATION

Object *Object_Pointer_Create();
void Object_Pointer_Destroy(Object *obj);

// this function DO NOT TAKE possession of Type *pointed_type (you do not have to pass a copy)
Object *Object_Pointer_Create(Type *pointed_type) {
  assert(pointed_type != NULL);

  typed_data(PointerObjectData);
  data->index = NullMemTableIndex;
  data->pointed_type = type_copy(pointed_type);

  // TODO: I do not like: 
  //  - is_strong = False;
  //  - file_info_create_null();
  return Object_Create(
    POINTER_OBJ, 
    POINTER_SIZE, 
    data, 
    type_create_pointer_type(type_copy(pointed_type), False, file_info_create_null()));
}

void Object_Pointer_Destroy(Object *obj) {
  assert(Object_Is_Pointer(obj));

  casted_data(PointerObjectData, obj);
  free(data);
}