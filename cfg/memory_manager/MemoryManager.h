#pragma once

#include "VariableMemoryMapping.h"
#include "MemoryTable.h"
#include "Variable.h"
#include "Definitions.h"


// MEMORY MANAGER

typedef struct {
  MemoryTable mem_table;
  VariableMemoryMapping var_table;
} MemoryManager;

MemTableIndex MM_Alloc(ByteSize);
Object MM_Access(MemTableIndex);
// Object MM_AccessArrayElement(MemTableIndex, ArrayIndex);
void MM_CreateVariable(Variable);
void MM_DestroyVariable(Variable);
