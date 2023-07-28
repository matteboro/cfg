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

int64_t int_increment = 0;

Object *Object_Create_From_Type(Type *real_type) {
  assert(real_type != NULL);

  if (type_is_integer(real_type)) {
    return Object_Integer_Create_Init(Integer_Create(int_increment++));
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

// PRINT

void Object_Print(Object *obj, FILE *file) {
  assert(obj != NULL);
  
  if(obj->type ==  INTEGER_OBJ) {
    Object_Integer_Print(obj, file);
  }
  else if(obj->type == STRING_OBJ) {
    Object_String_Print(obj, file);
  }
  else if(obj->type == STRUCT_OBJ) {
    Object_Struct_Print(obj, file);
  }
  else if(obj->type == POINTER_OBJ) {
    Object_Pointer_Print(obj, file);
  }
  else if(obj->type == ARRAY_OBJ) {
    fprintf(file, "ARRAY_OBJ");
  }
  else {
    UNREACHABLE();
  }
}

// OBJECT ARRAY

ObjectArray *ObjectArray_Create(size_t size) {
  ObjectArray *array = (ObjectArray *) malloc(sizeof(ObjectArray));
  //fprintf(stdout, "creating object array with size: %lu\n", size);
  array->size = size;
  array->curr = 0;
  array->objects = (Object **) malloc(sizeof(Object *)*size);
  for (size_t i=0; i<size; ++i) {
    array->objects[i] = NULL;
  }
  return array;
}

void ObjectArray_Destroy(ObjectArray *array) { 
  assert(array != NULL);
  for (size_t i=0; i<array->curr; ++i) {
    Object_Destroy(array->objects[i]);
  }
  free(array->objects);
  free(array);
}

void __ObjectArray_ShallowDestroy(ObjectArray *array) { 
  assert(array != NULL);

  free(array->objects);
  free(array);
}

bool ObjectArray_Empty(ObjectArray *array) {
  assert(array != NULL);
  return array->curr == 0 && array->size != 0;
}

bool ObjectArray_Full(ObjectArray *array){ 
  assert(array != NULL);
  return array->size == array->curr;
}

void ObjectArray_Push(ObjectArray *array, Object *obj) { 
  assert(array != NULL);
  assert(!ObjectArray_Full(array));

  // Object_Print(obj, stdout); fprintf(stdout, "\n");

  array->objects[array->curr] = obj;
  ++array->curr;
}

size_t ObjectArray_AvailableSpace(ObjectArray *array) {
  assert(array != NULL);
  return array->size - array->curr;
}

size_t ObjectArray_Capacity(ObjectArray *array) {
  assert(array != NULL);
  return array->size;
}

size_t ObjectArray_Size(ObjectArray *array) {
  assert(array != NULL);
  return array->curr;
}

// WARNING: after the use of this function pushed_array can not be used
//          anymore, its resources are taken from the dest array
void ObjectArray_PushArray(ObjectArray *dest, ObjectArray *pushed_array) {
  assert(ObjectArray_Full(pushed_array));
  assert(ObjectArray_AvailableSpace(dest) >= ObjectArray_Size(pushed_array));

  for (size_t i=0; i < ObjectArray_Size(pushed_array); ++i) {
    ObjectArray_Push(dest, ObjectArray_GetAt(pushed_array, i));
  }

  __ObjectArray_ShallowDestroy(pushed_array);
}

Object *ObjectArray_GetAt(ObjectArray *array, size_t index) {
  assert(array != NULL);
  assert(array->curr > index);

  return array->objects[index];
}

void ObjectArray_Print(ObjectArray *array, FILE *file) {
  assert(array != NULL);

  for (size_t i=0; i < ObjectArray_Size(array); ++i) {
    Object_Print(ObjectArray_GetAt(array, i), file);
    fprintf(file, " ");
  }  
  fprintf(file, "\n");
}