#pragma once

#include "Variable.h"
#include "Definitions.h"
#include "assert.h"

#define VMM_ASSERT(map) assert(map.map != NULL)

typedef struct {
  MemTableIndex *map;
  size_t size;
} VariableMemoryMapping;

VariableMemoryMapping VMM_Init(VariableIndex max_var_idx);
void VMM_Close(VariableMemoryMapping map);

MemTableIndex VMM_Get(VariableMemoryMapping map, Variable var);
void VMM_Set(VariableMemoryMapping map, Variable var, MemTableIndex idx);
void VMM_Clear(VariableMemoryMapping map, Variable var);

// IMPLEMENTATION

VariableMemoryMapping VMM_Init(VariableIndex max_var_idx) {

  assert(max_var_idx > 0);

  MemTableIndex *i_map = (MemTableIndex *) malloc(sizeof(MemTableIndex)*max_var_idx);
  for (size_t i=0; i < max_var_idx; ++i) {
    i_map[i] = 0;
  }

  VariableMemoryMapping map = { 
    .map = i_map, 
    .size = max_var_idx 
  };

  return map;
}

void VMM_Close(VariableMemoryMapping map) {
  VMM_ASSERT(map);

  free(map.map);
  return;
}

MemTableIndex VMM_Get(VariableMemoryMapping map, Variable var) {
  VMM_ASSERT(map);
  assert(map.size > var.var_idx);

  return map.map[var.var_idx];
}

void VMM_Set(VariableMemoryMapping map, Variable var, MemTableIndex idx) {
  VMM_ASSERT(map);
  assert(map.size > var.var_idx);
  assert(idx != 0);

  map.map[var.var_idx] = idx;
}

void VMM_Clear(VariableMemoryMapping map, Variable var) {
  VMM_ASSERT(map);
  assert(map.size > var.var_idx);

  map.map[var.var_idx] = 0;
}
