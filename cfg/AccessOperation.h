#pragma once

#include "memory_manager/Variable.h"

// FORWARD DECLARATION
typedef struct CFGOperand_i CFGOperand;
void CFGOperand_Destroy(CFGOperand *cfg_op);

// DEFINITION

typedef struct {
  Variable var;
  CFGOperand *op_idx;  // can be NULL
  bool is_pointer_access;
} AccessOperation;

AccessOperation *AccessOperation_Create_Variable_Access(Variable var);
AccessOperation *AccessOperation_Create_Indexed_Variable_Access(Variable var, CFGOperand *op_idx);
AccessOperation *AccessOperation_Create_Pointer_Access(Variable var);
AccessOperation *AccessOperation_Create_Indexed_Pointer_Access(Variable var, CFGOperand *op_idx);

void AccessOperation_Destroy(AccessOperation *access_op);

// CREATE

AccessOperation *__AccessOperation_Create(Variable var, CFGOperand *op_idx, bool is_pointer_access) {
  AccessOperation *access_op = (AccessOperation *) malloc(sizeof(AccessOperation));
  access_op->var = var;
  access_op->op_idx = op_idx;
  access_op->is_pointer_access = is_pointer_access;
  return access_op;
}

AccessOperation *AccessOperation_Create_Variable_Access(Variable var) {
  return __AccessOperation_Create(var, NULL, False);
}

AccessOperation *AccessOperation_Create_Indexed_Variable_Access(Variable var, CFGOperand *op_idx) {
  assert(op_idx != NULL);
  return __AccessOperation_Create(var, op_idx, False);
}

AccessOperation *AccessOperation_Create_Pointer_Access(Variable var) {
  return __AccessOperation_Create(var, NULL, True);
}

AccessOperation *AccessOperation_Create_Indexed_Pointer_Access(Variable var, CFGOperand *op_idx) {
  assert(op_idx != NULL);
  return __AccessOperation_Create(var, op_idx, True);
}

// DESTROY

void AccessOperation_Destroy(AccessOperation *access_op) {
  if (access_op->op_idx != NULL) {
    CFGOperand_Destroy(access_op->op_idx);
  }
  free(access_op);
  return;
}
