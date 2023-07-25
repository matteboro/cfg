#pragma once

#include "VariableMemoryMapping.h"
#include "MemoryTable.h"
#include "Variable.h"
#include "Definitions.h"

// DEFINITION

typedef struct {
  MemoryTable mem_table;
  VariableMemoryMapping var_table;
} MemoryManager;

MemoryManager MM_Init(VariableIndex max_var_idx);
void MM_Close(MemoryManager);

MemTableIndex MM_Alloc(MemoryManager);
Object *MM_Access(MemoryManager, MemTableIndex);

void MM_Declare_Variable(MemoryManager, Variable);
void MM_Destroy_Variable(MemoryManager, Variable);

// IMPLEMENTATION

MemoryManager MM_Init(VariableIndex max_var_idx) {
  MemoryTable mem_table = MT_Init();
  VariableMemoryMapping var_table = VMM_Init(max_var_idx);
  MemoryManager mem_man  = {
    .mem_table = mem_table,
    .var_table = var_table
  };
  return mem_man;
}

void MM_Close(MemoryManager mem_man) {
  VMM_Close(mem_man.var_table);
  MT_Close(mem_man.mem_table);
}

MemTableIndex MM_Alloc(MemoryManager mem_man) {

}

Object *MM_Access(MemoryManager mem_man, MemTableIndex mt_idx) {

}

void MM_Declare_Variable(MemoryManager mem_man, Variable var) {
  assert(VMM_Variable_Alive(mem_man.var_table, var));

  Object *init_var_obj = Object_Create_From_Type(var.type);
  MemTableIndex mt_idx = MT_Insert(mem_man.mem_table, init_var_obj);
  assert(mt_idx != NullMemTableIndex);

  VMM_Set(mem_man.var_table, var, mt_idx);
}

void MM_Destroy_Variable(MemoryManager mem_man, Variable var) {
  assert(!VMM_Variable_Alive(mem_man.var_table, var));

  MemTableIndex mt_idx = VMM_Get(mem_man.var_table, var);
  VMM_Clear(mem_man.var_table, var);
  MT_Remove(mem_man.mem_table, mt_idx);
}


