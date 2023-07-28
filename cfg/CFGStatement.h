#pragma once

#include "CFGExpression.h"
#include "memory_manager/Variable.h"

typedef enum {
  ASSIGNMENT_CFG_STMNT,
  DECLARATION_CFG_STMNT,
  UNDECLARATION_CFG_STMNT,
} CFGStatementType;

typedef void CFGStatementData;

typedef struct {
  CFGStatementType type;
  CFGStatementData *data;
} CFGStatement;

typedef struct {
  AccessOperation *left_access;
  CFGExpression *value_expression;
} AssignmentCFGStatementData;

typedef struct {
  Variable var;
} DeclarationCFGStatementData;

typedef struct {
  Variable var;
} UndeclarationCFGStatementData;

CFGStatement *CFGStatement_Create_Assignment(AccessOperation *left_access, CFGExpression *value_expression);
CFGStatement *CFGStatement_Create_Declaration(Variable var);
CFGStatement *CFGStatement_Create_Undeclaration(Variable var);
void CFGStatement_Destroy(CFGStatement *cfg_stmnt);
void CFGStatement_Print(CFGStatement *cfg_stmnt, FILE *file);

// LIST(CFGStatement, CFGStatement, CFGStatement_Destroy, CFGStatement_Print)

// CREATE

CFGStatement *__CFGStatement_Create(CFGStatementType type, CFGStatementData *data) {
  assert(data != NULL);
  CFGStatement *cfg_stmnt = (CFGStatement *) malloc(sizeof(CFGStatement));
  cfg_stmnt->type = type;
  cfg_stmnt->data = data;
  return cfg_stmnt;
}

CFGStatement *CFGStatement_Create_Assignment(AccessOperation *left_access, CFGExpression *value_expression) {
  assert(left_access != NULL);
  assert(value_expression != NULL);
  typed_data(AssignmentCFGStatementData);
  data->left_access = left_access;
  data->value_expression = value_expression;
  return __CFGStatement_Create(ASSIGNMENT_CFG_STMNT, data);
}

CFGStatement *CFGStatement_Create_Declaration(Variable var) {
  // TODO: assert(Variable_Is_Valid(var));
  typed_data(DeclarationCFGStatementData);
  data->var = var;
  return __CFGStatement_Create(DECLARATION_CFG_STMNT, data);
}

CFGStatement *CFGStatement_Create_Undeclaration(Variable var) {
  // TODO: assert(Variable_Is_Valid(var));
  typed_data(UndeclarationCFGStatementData);
  data->var = var;
  return __CFGStatement_Create(UNDECLARATION_CFG_STMNT, data);
}

// DESTROY

void __CFGStatement_Destroy_Assignment(CFGStatement *cfg_stmnt) {
  casted_data(AssignmentCFGStatementData, cfg_stmnt);
  AccessOperation_Destroy(data->left_access);
  CFGExpression_Destroy(data->value_expression);
  free(data);
}

void __CFGStatement_Destroy_Declaration(CFGStatement *cfg_stmnt) {
  casted_data(DeclarationCFGStatementData, cfg_stmnt);
  free(data);
}

void __CFGStatement_Destroy_Undeclaration(CFGStatement *cfg_stmnt) {
  casted_data(UndeclarationCFGStatementData, cfg_stmnt);
  free(data);
}

void CFGStatement_Destroy(CFGStatement *cfg_stmnt) {
  assert(cfg_stmnt != NULL);

  if (cfg_stmnt->type == ASSIGNMENT_CFG_STMNT) {
    __CFGStatement_Destroy_Assignment(cfg_stmnt);
  } 
  else if (cfg_stmnt->type == DECLARATION_CFG_STMNT) {
    __CFGStatement_Destroy_Declaration(cfg_stmnt);
  }
  else if (cfg_stmnt->type == UNDECLARATION_CFG_STMNT) {
    __CFGStatement_Destroy_Undeclaration(cfg_stmnt);
  } 
  else {
    UNREACHABLE();
  }

  free(cfg_stmnt);
}

// PRINT

void __CFGStatement_Print_Assignment(CFGStatement *cfg_stmnt, FILE *file) {
  casted_data(AssignmentCFGStatementData, cfg_stmnt);
  AccessOperation_Print(data->left_access, file);
  fprintf(file, " := ");
  CFGExpression_Print(data->value_expression, file);
}

void __CFGStatement_Print_Declaration(CFGStatement *cfg_stmnt, FILE *file) {
  casted_data(DeclarationCFGStatementData, cfg_stmnt);
  fprintf(file, "decl ");
  Variable_Print(data->var, file);
}

void __CFGStatement_Print_Undeclaration(CFGStatement *cfg_stmnt, FILE *file) {
  casted_data(UndeclarationCFGStatementData, cfg_stmnt);
  fprintf(file, "undecl ");
  Variable_Print(data->var, file);
}



void CFGStatement_Print(CFGStatement *cfg_stmnt, FILE *file) {
  assert(cfg_stmnt != NULL);

  if (cfg_stmnt->type == ASSIGNMENT_CFG_STMNT) {
    __CFGStatement_Print_Assignment(cfg_stmnt, file);
  } 
  else if (cfg_stmnt->type == DECLARATION_CFG_STMNT) {
    __CFGStatement_Print_Declaration(cfg_stmnt, file);
  }
  else if (cfg_stmnt->type == UNDECLARATION_CFG_STMNT) {
    __CFGStatement_Print_Undeclaration(cfg_stmnt, file);
  } 
  else {
    UNREACHABLE();
  }
}