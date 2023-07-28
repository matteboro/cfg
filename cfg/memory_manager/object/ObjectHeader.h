#pragma once

#include "../Definitions.h"
#include "../../../expr/type.h"

#undef typed_data
#undef casted_data
#define typed_data(type) type* data = (type *) malloc(sizeof(type)) 
#define casted_data(type, obj) type* data = (type *) obj->data;

#define INTEGER_SIZE 8
#define STRING_SIZE 8
#define POINTER_SIZE 8

// DEFINITION

typedef enum {
  INTEGER_OBJ,
  STRING_OBJ,
  POINTER_OBJ,
  COMPOSED_OBJ,
  // define above
  COUNT_OBJ,
} ObjectType;

typedef void ObjectData;

struct Object_d {
  ObjectType type;
  ByteSize size;
  ObjectData *data;
  Type *real_type;
};

Object *Object_Create(ObjectType type, ByteSize size, ObjectData *data, Type *real_type);
Object *Object_Create_From_Type(Type *real_type);
void Object_Destroy(Object *obj);

bool Object_Is_Of_Type(Object *obj, ObjectType obj_t);
bool Object_Is_Integer(Object *obj);
bool Object_Is_String(Object *obj);
bool Object_Is_Struct(Object *obj);
bool Object_Is_Array(Object *obj);
bool Object_Is_Pointer(Object *obj);
bool Object_Is_Composed(Object *obj);

void Object_Print(Object *obj, FILE *file);

// OBJECT ARRAY

typedef struct {
  size_t size;
  size_t curr;
  Object **objects;
} ObjectArray;

ObjectArray *ObjectArray_Create(size_t size);
void ObjectArray_Destroy(ObjectArray *array);

void ObjectArray_Push(ObjectArray *array, Object *obj);
void ObjectArray_PushArray(ObjectArray *dest, ObjectArray *pushed_array);

Object *ObjectArray_GetAt(ObjectArray *array, size_t index);

bool ObjectArray_Empty(ObjectArray *array);
bool ObjectArray_Full(ObjectArray *array);

size_t ObjectArray_AvailableSpace(ObjectArray *array);
size_t ObjectArray_Capacity(ObjectArray *array);
size_t ObjectArray_Size(ObjectArray *array);

void ObjectArray_Print(ObjectArray *array, FILE* file);