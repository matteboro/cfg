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
Statement *stmnt_dealloc(Statement *stmnt);
Statement *stmnt_print(Statement *stmnt, FILE *file);

enum StatementType_e {
  ASSIGNMENT_STMNT,
  DECLARATION_STMNT,
  BLOCK_STMNT,
  IF_ELSE_STMTN,
  WHILE_STMNT,
  RETURN_STMNT,
};

struct Statement_s {
  StatementPayload payload;
  StatementType type;
  void (*print)(Statement *, FILE *);
  void (*dealloc)(Statement *);
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

// BLOCK STATEMENT (multiple STATEMENTs)
// payload {StatementList *statements}

// IF_ELSE (with ELIF chains)
// payload {Expression *condition, Statement *if_body, Statement *else_body}

// WHILE
// payload {Expression *condition, Statement *body}

// RETURN
// payload {Expression *ret_value}