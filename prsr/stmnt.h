#pragma once

#include "../expr/expr.h"
#include "assgnbl.h"

#define STMNT_ALLOC_PAYLOAD(type) type *payload = (type *)malloc(sizeof(type))
#define STMNT_CAST_PAYLOAD(type, obj) type *payload = (type *)obj->payload;
#define STMNT_ALLOC(type) (type *)malloc(sizeof(type))

enum StatementType_e;
typedef enum StatementType_e StatementType;

struct Statement_s;
typedef struct Statement_s Statement;

typedef void * StatementPayload;

Statement *stmnt_create(StatementPayload payload, StatementType type);
void stmnt_dealloc(Statement *stmnt);
void stmnt_print(Statement *stmnt, FILE *file);

enum StatementType_e {
  ASSIGNMENT_STMNT,
  DECLARATION_STMNT,
  BLOCK_STMNT,
  IF_ELSE_STMNT,
  WHILE_STMNT,
  RETURN_STMNT,
  COUNT_STMNT,
};

struct Statement_s {
  StatementPayload payload;
  StatementType type;
};

Statement *stmnt_create(StatementPayload payload, StatementType type) {
  Statement *stmnt = STMNT_ALLOC(Statement);
  stmnt->payload = payload;
  stmnt->type = type;
  return stmnt;
}

// STATEMENT LIST

LIST(stmnt, Statement, stmnt_dealloc, stmnt_print)

// ASSIGNMENT

struct AssignmentPayload_s;
typedef AssignmentPayload_s AssignmentPayload;

Statement *stmnt_create_assignment(AssignableElement *, Expression *);
void stmnt_print_assignment(Statement *, FILE *);
void stmnt_dealloc_assignment(Statement *);

struct AssignmentPayload_s {
  AssignableElement *assgnbl;
  Expression *value;
};

Statement *stmnt_create_assignment(AssignableElement *assgnbl, Expression *value) {
  STMNT_ALLOC_PAYLOAD(AssignmentPayload);
  payload->assgnbl = assgnbl;
  payload->value = value;
  return stmnt_create(payload, ASSIGNMENT_STMNT);
}

void stmnt_print_assignment(Statement *stmnt, FILE *file) {
  fprintf(file, "ASSIGNMENT");
}

void stmnt_dealloc_assignment(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(AssignmentPayload, stmnt);
  assgnbl_dealloc(payload->assgnbl);
  expr_dealloc_expression(payload->value);
  free(payload);
}

// DECLARATION

struct DeclarationPayload_s;
typedef DeclarationPayload_s DeclarationPayload;

Statement *stmnt_create_declaration(AssignableElement *, Expression *);
void stmnt_print_declaration(Statement *, FILE *);
void stmnt_dealloc_declaration(Statement *);

struct DeclarationPayload_s {
  NameTypeBinding *nt_bind;
  ExpressionList *init_values;
};

Statement *stmnt_create_declaration(NameTypeBinding *nt_bind, ExpressionList *init_values) {
  STMNT_ALLOC_PAYLOAD(DeclarationPayload);
  payload->nt_bind = nt_bind;
  payload->init_values = init_values;
  return stmnt_create(payload, DECLARATION_STMNT);
}

void stmnt_print_declaration(Statement *stmnt, FILE *file) {
  fprintf(file, "DECLARATION");
}

void stmnt_dealloc_declaration(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(DeclarationPayload, stmnt);
  nt_bind_dealloc(payload->nt_bind);
  expr_list_dealloc(payload->init_values);
  free(payload);
}

// IF_ELSE (with ELIF chains)
// payload {Expression *condition, Statement *if_body, Statement *else_body}

struct IfElsePayload_s;
typedef IfElsePayload_s IfElsePayload;

Statement *stmnt_create_if_else(AssignableElement *, Expression *);
void stmnt_print_if_else(Statement *, FILE *);
void stmnt_dealloc_if_else(Statement *);

struct IfElsePayload_s {
  Expression *condition;
  Statement *body;
};

Statement *stmnt_create_if_else( Expression *condition, Statement *body) {
  STMNT_ALLOC_PAYLOAD(IfElsePayload);
  payload->condition = condition;
  payload->body = body;
  return stmnt_create(payload, IF_ELSE_STMNT);
}

void stmnt_print_if_else(Statement *stmnt, FILE *file) {
  fprintf(file, "IF_ELSE");
}

void stmnt_dealloc_if_else(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(IfElsePayload, stmnt);
  expr_dealloc_expression(payload->condition);
  stmnt_dealloc(payload->body);
  free(payload);
}

// WHILE
// payload {Expression *condition, Statement *body}

struct WhilePayload_s;
typedef WhilePayload_s WhilePayload;

Statement *stmnt_create_while(AssignableElement *, Expression *);
void stmnt_print_while(Statement *, FILE *);
void stmnt_dealloc_while(Statement *);

struct WhilePayload_s {
  Expression *condition;
  Statement *body;
};

Statement *stmnt_create_while( Expression *condition, Statement *body) {
  STMNT_ALLOC_PAYLOAD(WhilePayload);
  payload->condition = condition;
  payload->body = body;
  return stmnt_create(payload, WHILE_STMNT);
}

void stmnt_print_while(Statement *stmnt, FILE *file) {
  fprintf(file, "WHILE");
}

void stmnt_dealloc_while(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(WhilePayload, stmnt);
  expr_dealloc_expression(payload->condition);
  stmnt_dealloc(payload->body);
  free(payload);
}

// RETURN
// payload {Expression *ret_value}

struct ReturnPayload_s;
typedef ReturnPayload_s ReturnPayload;

Statement *stmnt_create_return(AssignableElement *, Expression *);
void stmnt_print_return(Statement *, FILE *);
void stmnt_dealloc_return(Statement *);

struct ReturnPayload_s {
  Expression *ret_value;
};

Statement *stmnt_create_return( Expression *ret_value) {
  STMNT_ALLOC_PAYLOAD(ReturnPayload);
  payload->ret_value = ret_value;
  return stmnt_create(payload, RETURN_STMNT);
}

void stmnt_print_return(Statement *stmnt, FILE *file) {
  fprintf(file, "RETURN");
}

void stmnt_dealloc_return(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(ReturnPayload, stmnt);
  expr_dealloc_expression(payload->ret_value);
  free(payload);
}

// BLOCK STATEMENT (multiple STATEMENTs)
// payload {StatementList *statements}

struct BlockPayload_s;
typedef BlockPayload_s BlockPayload;

Statement *stmnt_create_block(AssignableElement *, Expression *);
void stmnt_print_block(Statement *, FILE *);
void stmnt_dealloc_block(Statement *);

struct BlockPayload_s {
  StatementList *body;
};

Statement *stmnt_create_block(StatementList *body) {
  STMNT_ALLOC_PAYLOAD(BlockPayload);
  payload->body = body;
  return stmnt_create(payload, BLOCK_STMNT);
}

void stmnt_print_block(Statement *stmnt, FILE *file) {
  fprintf(file, "BLOCK");
}

void stmnt_dealloc_block(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(BlockPayload, stmnt);
  stmnt_list_dealloc(payload->body);
  free(payload);
}

// GENERAL DEFINITION

void (*stmnt_dealloc_funcs[])(Statement *) = {
  [ASSIGNMENT_STMNT] = stmnt_dealloc_assignment,
  [DECLARATION_STMNT] = stmnt_dealloc_declaration,
  [BLOCK_STMNT] = stmnt_dealloc_block,
  [IF_ELSE_STMNT] = stmnt_dealloc_if_else,
  [WHILE_STMNT] = stmnt_dealloc_while,
  [RETURN_STMNT] = stmnt_dealloc_return,
};

void stmnt_dealloc(Statement *stmnt) {
  assert(COUNT_STMNT == sizeof(stmnt_dealloc_funcs));
  stmnt_dealloc_funcs[stmnt->type](stmnt);
  free(stmnt);
}

void (*stmnt_print_funcs[])(Statement *, FILE *) = {
  [ASSIGNMENT_STMNT] = stmnt_print_assignment,
  [DECLARATION_STMNT] = stmnt_print_declaration,
  [BLOCK_STMNT] = stmnt_print_block,
  [IF_ELSE_STMNT] = stmnt_print_if_else,
  [WHILE_STMNT] = stmnt_print_while,
  [RETURN_STMNT] = stmnt_print_return,
};

void stmnt_print(Statement *stmnt, FILE *file) {
  assert(COUNT_STMNT == sizeof(stmnt_print_funcs));
  stmnt_print_funcs[stmnt->type](stmnt, file);
}
