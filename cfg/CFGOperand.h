#pragma once

#include "Literal.h"
#include "AccessOperation.h"

typedef enum {
  LITERAL_CFG_OP,
  ACCESS_OP_CFG_OP,
} CFGOperandType;

typedef void CFGOperandData;

typedef struct CFGOperand_i {
  CFGOperandType type;
  CFGOperandData *data;
} CFGOperand;

typedef struct {
  Literal *literal;
} LiteralCFGOpernadData;

typedef struct {
  AccessOperation *access_op;
} AccessOperationCFGOperandData;


CFGOperand *CFGOperand_Create_Literal(Literal *literal);
CFGOperand *CFGOperand_Create_AccessOperation(AccessOperation *access_op);

void CFGOperand_Destroy(CFGOperand *cfg_op);

// CREATE

CFGOperand *__CFGOperand_Create(  CFGOperandType type, CFGOperandData *data) {
  assert(data != NULL);
  CFGOperand *cfg_op = (CFGOperand *) malloc(sizeof(CFGOperand));
  cfg_op->data = data;
  cfg_op->type = type;
  return cfg_op;
}

CFGOperand *CFGOperand_Create_Literal(Literal *literal) {
  assert(literal != NULL);
  typed_data(LiteralCFGOpernadData);
  data->literal = literal;
  return __CFGOperand_Create(LITERAL_CFG_OP, data);
}

CFGOperand *CFGOperand_Create_AccessOperation(AccessOperation *access_op) {
  assert(access_op != NULL);
  typed_data(AccessOperationCFGOperandData);
  data->access_op = access_op;
  return __CFGOperand_Create(ACCESS_OP_CFG_OP, data);
}

// DESTROY

void __CFGOperand_Destroy_Literal(CFGOperand *cfg_op) {
  casted_data(LiteralCFGOpernadData, cfg_op);
  Literal_Destroy(data->literal);
  free(data);
}

void __CFGOperand_Destroy_AccessOperation(CFGOperand *cfg_op) {
  casted_data(AccessOperationCFGOperandData, cfg_op);
  AccessOperation_Destroy(data->access_op);
  free(data);
}

void CFGOperand_Destroy(CFGOperand *cfg_op) {
  assert(cfg_op != NULL);
  if (cfg_op->type == LITERAL_CFG_OP) {
    __CFGOperand_Destroy_Literal(cfg_op);
  } 
  else if (cfg_op->type == ACCESS_OP_CFG_OP) {
    __CFGOperand_Destroy_AccessOperation(cfg_op);
  } 
  else {
    UNREACHABLE();
  }
  free(cfg_op);
}