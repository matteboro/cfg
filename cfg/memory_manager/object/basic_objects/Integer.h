#pragma once 

#include <stdint.h>

typedef struct {
  int64_t value;
} Integer;


Integer Integer_Create(int64_t value);


Integer Integer_Create(int64_t value) {
  Integer i = {
    .value = value
  };
  return i;
}