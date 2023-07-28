#pragma once

#include <stdio.h>

// DEFINITIONS

typedef struct {
  char *string;
} String;

String String_Create();
void String_Destroy(String string);

// IMPLEMENTATION

String String_Create(char *cstring) {
  String string = { .string = cstring };
  return string;
}

void String_Destroy(String string) {
  assert(string.string != NULL);
  
  free(string.string);
}

void String_Print(String string, FILE *file) {
  assert(string.string != NULL);
  
  fprintf(file, "\"%s\"", string.string);
}