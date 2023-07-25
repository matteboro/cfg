#pragma once

#include "IntegerObject.h"
#include "StringObject.h"
#include "PointerObject.h"
#include "ArrayObject.h"
#include "StructObject.h"


// CREATE / DESTROY

Object *Object_Create(ObjectType type, ByteSize size, ObjectData *data, Type *real_type) {
  assert(type < COUNT_OBJ);
  assert(real_type != NULL);
  assert(data != NULL);

  Object *obj = (Object *) malloc(sizeof(Object));
  obj->type = type;
  obj->data = data;
  obj->size = size;
  obj->real_type = real_type;
  return obj;
}

// this function DOES NOT TAKE possession of Type *real_type (you do not have to pass a copy)
Object *Object_Create_From_Type(Type *real_type) {
  assert(real_type != NULL);

  if (type_is_integer(real_type)) {
    return Object_Integer_Create();
  }   
  else if (type_is_string(real_type)) {
    return Object_String_Create();
  }
  else if (type_is_struct(real_type)) {
    return Object_Struct_Create(type_struct_get_struct_decl(real_type));
  }
  else if (type_is_array(real_type)) {
    return Object_Array_Create(
      (ArraySize) type_array_get_size(real_type), 
      type_extract_ultimate_type(real_type));
  }
  else if (type_is_pointer(real_type)) {
    return Object_Pointer_Create(type_extract_ultimate_type(real_type));
  }
  else {
    assert(False);
  }
}

void Object_Destroy(Object *obj) {
  assert(obj != NULL);

  switch(obj->type) {
    case INTEGER_OBJ: Object_Integer_Destroy(obj); break;
    case STRING_OBJ:  Object_String_Destroy(obj); break;
    case STRUCT_OBJ:  Object_Struct_Destroy(obj); break;
    case POINTER_OBJ: Object_Pointer_Destroy(obj); break;
    case ARRAY_OBJ:   Object_Array_Destroy(obj); break;
    default:
      assert(False); break;
  }
  type_dealloc(obj->real_type);
  free(obj);
}

// OBJECT IS OF TYPE

bool Object_Is_Of_Type(Object *obj, ObjectType obj_t) {
  assert(obj != NULL);
  assert(obj_t < COUNT_OBJ);
  return obj->type == obj_t;
}

bool Object_Is_Integer(Object *obj) {
  return Object_Is_Of_Type(obj, INTEGER_OBJ);
}

bool Object_Is_String(Object *obj) {
  return Object_Is_Of_Type(obj, STRING_OBJ);
}

bool Object_Is_Struct(Object *obj) {
  return Object_Is_Of_Type(obj, STRUCT_OBJ);
}

bool Object_Is_Array(Object *obj) {
  return Object_Is_Of_Type(obj, ARRAY_OBJ);
}

bool Object_Is_Pointer(Object *obj) {
  return Object_Is_Of_Type(obj, POINTER_OBJ);
}

