#pragma once

#include "./object/Object.h"
#include "Definitions.h"

// MemTableIndex --> Object *

typedef struct {
  
} MemoryTable;

MemoryTable MT_Init();
void MT_Close(MemoryTable);

MemTableIndex MT_Insert(MemoryTable, Object *);
void MT_Remove(MemoryTable, MemTableIndex);


