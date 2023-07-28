#pragma once

#include "ObjectHeader.h"
#include "./basic_objects/String.h"

char *empty_string() {
  char *empty_string = (char *) malloc(sizeof(char));
  empty_string[0] = '\0';
  return empty_string;
}

// DEFINITION

typedef struct {
  String string;
} StringObjectData;

Object *Object_String_Create();
Object *Object_String_Create_Init(String string);
void Object_String_Destroy(Object *obj);

// IMPLEMENTATION

Object *Object_String_Create() {
  typed_data(StringObjectData);
  String string = String_Create(empty_string());
  data->string = string;
  return Object_Create(STRING_OBJ, STRING_SIZE, data, type_create_generic_string_type());
}

Object *Object_String_Create_Init(String string) {
  typed_data(StringObjectData);
  data->string = string;
  return Object_Create(STRING_OBJ, STRING_SIZE, data, type_create_generic_string_type()); 
}

void Object_String_Destroy(Object *obj){ 
  assert(Object_Is_String(obj));

  casted_data(StringObjectData, obj);
  String_Destroy(data->string);
  free(data);
}

void Object_String_Print(Object *obj, FILE *file) {
  assert(Object_Is_String(obj));

  casted_data(StringObjectData, obj);
  String_Print(data->string, file);
}