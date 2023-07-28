#pragma once 

#include <stdint.h>
#include <stdio.h>

typedef struct {
  int64_t value;
} Integer;


Integer Integer_Create(int64_t value);
void Integer_Print(Integer integer, FILE *file);


Integer Integer_Create(int64_t value) {
  Integer i = {
    .value = value
  };
  return i;
}

void Integer_Print(Integer integer, FILE *file) {
  fprintf(file, "%ld", integer.value);
}