#pragma once

#include "./object/Object.h"
#include "Definitions.h"

// MemTableIndex --> Object *

typedef struct {
  
} MemoryTable;

MemoryTable MT_Init();
void MT_Close(MemoryTable);

void MT_Insert(MemTableIndex, Object *);
void MT_Remove(MemTableIndex);


