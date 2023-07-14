#pragma once

#include "../expr/expr.h"
#include "assgnbl.h"

#define STMNT_ALLOC_PAYLOAD(type) type *payload = (type *)malloc(sizeof(type))
#define STMNT_CAST_PAYLOAD(type, obj) type *payload = (type *)obj->payload
#define STMNT_ALLOC(type) (type *)malloc(sizeof(type))
#define print_spaces(n, file) {for (size_t spaces_counter=0; spaces_counter<n; ++spaces_counter) fprintf(file, " ");}


enum StatementType_e;
typedef enum StatementType_e StatementType;

struct Statement_s;
typedef struct Statement_s Statement;

typedef void * StatementPayload;

Statement *stmnt_create(StatementPayload payload, StatementType type);
void stmnt_dealloc(Statement *stmnt);
void stmnt_print(Statement *stmnt, FILE *file);
void stmnt_print_ident(Statement *stmnt, FILE *file, size_t ident);

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
typedef struct AssignmentPayload_s AssignmentPayload;

Statement *stmnt_create_assignment(AssignableElement *, Expression *);
void stmnt_print_assignment_ident(Statement *, FILE *, size_t);
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

void stmnt_print_assignment_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(AssignmentPayload, stmnt);

  print_spaces(ident, file);
  assgnbl_print(payload->assgnbl, file);
  fprintf(file, " = ");
  expr_print_expression(payload->value, file);
}

void stmnt_dealloc_assignment(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(AssignmentPayload, stmnt);
  assgnbl_dealloc(payload->assgnbl);
  expr_dealloc_expression(payload->value);
  free(payload);
}

// DECLARATION

struct DeclarationPayload_s;
typedef struct DeclarationPayload_s DeclarationPayload;

Statement *stmnt_create_declaration(NameTypeBinding *, ExpressionList *);
void stmnt_print_declaration_ident(Statement *, FILE *, size_t);
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

void stmnt_print_declaration_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(DeclarationPayload, stmnt);

  print_spaces(ident, file);
  nt_bind_print(payload->nt_bind, file);
  if (payload->init_values != NULL) {
    fprintf(file, " = ");
    if (expr_list_size(payload->init_values) > 1) {
      fprintf(file, "[ ");
      expr_list_print(payload->init_values, file);
      fprintf(file, " ]");
    } else {
      expr_list_print(payload->init_values, file);
    }
  }
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
typedef struct IfElsePayload_s IfElsePayload;

Statement *stmnt_create_if_else(Expression *, Statement *, Statement *);
void stmnt_print_if_else_ident(Statement *, FILE *, size_t);
void stmnt_dealloc_if_else(Statement *);

struct IfElsePayload_s {
  Expression *condition;
  Statement *if_body;
  Statement *else_body;
};

Statement *stmnt_create_if_else(Expression *condition, Statement *if_body, Statement *else_body) {
  STMNT_ALLOC_PAYLOAD(IfElsePayload);
  payload->condition = condition;
  payload->if_body = if_body;
  payload->else_body = else_body;
  return stmnt_create(payload, IF_ELSE_STMNT);
}

void stmnt_print_if_else_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(IfElsePayload, stmnt);

  print_spaces(ident, file);
  fprintf(file, "if ");
  expr_print_expression(payload->condition, file);
  fprintf(file, " {\n");

  stmnt_print_ident(payload->if_body, file, ident+2);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "}");

  if (payload->else_body->type == IF_ELSE_STMNT) {
    fprintf (file, " else \n");
    stmnt_print_ident(payload->else_body, file, ident);
    fprintf(file, "\n");
  } else {
    fprintf (file, " else {\n");
    stmnt_print_ident(payload->else_body, file, ident+2);
    fprintf(file, "\n");

    print_spaces(ident, file);
    fprintf(file, "}");
  }
}

void stmnt_dealloc_if_else(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(IfElsePayload, stmnt);
  expr_dealloc_expression(payload->condition);
  stmnt_dealloc(payload->if_body);
  stmnt_dealloc(payload->else_body);
  free(payload);
}

// WHILE
// payload {Expression *condition, Statement *body}

struct WhilePayload_s;
typedef struct WhilePayload_s WhilePayload;

Statement *stmnt_create_while(Expression *, Statement *);
void stmnt_print_while_ident(Statement *, FILE *, size_t);
void stmnt_dealloc_while(Statement *);

struct WhilePayload_s {
  Expression *condition;
  Statement *body;
};

Statement *stmnt_create_while(Expression *condition, Statement *body) {
  STMNT_ALLOC_PAYLOAD(WhilePayload);
  payload->condition = condition;
  payload->body = body;
  return stmnt_create(payload, WHILE_STMNT);
}

void stmnt_print_while_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(WhilePayload, stmnt);

  print_spaces(ident, file);
  fprintf(file, "while ");
  expr_print_expression(payload->condition, file);
  fprintf(file, " {\n");

  stmnt_print_ident(payload->body, file, ident+2);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "}");
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
typedef struct ReturnPayload_s ReturnPayload;

Statement *stmnt_create_return(Expression *);
void stmnt_print_return_ident(Statement *, FILE *, size_t);
void stmnt_dealloc_return(Statement *);

struct ReturnPayload_s {
  Expression *ret_value;
};

Statement *stmnt_create_return(Expression *ret_value) {
  STMNT_ALLOC_PAYLOAD(ReturnPayload);
  payload->ret_value = ret_value;
  return stmnt_create(payload, RETURN_STMNT);
}

void stmnt_print_return_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(ReturnPayload, stmnt);
  print_spaces(ident, file);
  fprintf(file, "return ");
  expr_print_expression(payload->ret_value, file);
}

void stmnt_dealloc_return(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(ReturnPayload, stmnt);
  expr_dealloc_expression(payload->ret_value);
  free(payload);
}

// BLOCK STATEMENT (multiple STATEMENTs)
// payload {StatementList *statements}

struct BlockPayload_s;
typedef struct BlockPayload_s BlockPayload;

Statement *stmnt_create_block(StatementList *);
void stmnt_print_block_ident(Statement *, FILE *, size_t);
void stmnt_dealloc_block(Statement *);

struct BlockPayload_s {
  StatementList *body;
};

Statement *stmnt_create_block(StatementList *body) {
  STMNT_ALLOC_PAYLOAD(BlockPayload);
  payload->body = body;
  return stmnt_create(payload, BLOCK_STMNT);
}

void stmnt_print_block_ident(Statement *stmnt, FILE *file, size_t ident) {

  STMNT_CAST_PAYLOAD(BlockPayload, stmnt);

  FOR_EACH(StatementList, stmnt_it, payload->body) {
    stmnt_print_ident(stmnt_it->node, file, ident);
    if (stmnt_it->next != NULL)
      fprintf(file, "\n");
  }
}

void stmnt_dealloc_block(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(BlockPayload, stmnt);
  stmnt_list_dealloc(payload->body);
  free(payload);
}

// GENERAL DEFINITION

#define STMNT_PRINT_SIGN   void (*)(Statement *, FILE *, size_t)
#define STMNT_DEALLOC_SIGN void (*)(Statement *)

enum {
  PRINT_FUNC   = 0,
  DEALLOC_FUNC = 1,
};

void *stmnt_funcs_map[][2] = {
  [ASSIGNMENT_STMNT]   = {stmnt_print_assignment_ident,   stmnt_dealloc_assignment},
  [DECLARATION_STMNT]  = {stmnt_print_declaration_ident,  stmnt_dealloc_declaration},
  [BLOCK_STMNT]        = {stmnt_print_block_ident,        stmnt_dealloc_block},
  [IF_ELSE_STMNT]      = {stmnt_print_if_else_ident,      stmnt_dealloc_if_else},
  [WHILE_STMNT]        = {stmnt_print_while_ident,        stmnt_dealloc_while},
  [RETURN_STMNT]       = {stmnt_print_return_ident,       stmnt_dealloc_return},
};

void (*stmnt_dealloc_funcs[])(Statement *) = {
  [ASSIGNMENT_STMNT] = stmnt_dealloc_assignment,
  [DECLARATION_STMNT] = stmnt_dealloc_declaration,
  [BLOCK_STMNT] = stmnt_dealloc_block,
  [IF_ELSE_STMNT] = stmnt_dealloc_if_else,
  [WHILE_STMNT] = stmnt_dealloc_while,
  [RETURN_STMNT] = stmnt_dealloc_return,
};

#define STMNT_SIZE_OF_DEALLOC_FUNCS sizeof(stmnt_dealloc_funcs)/sizeof(void (*)(Statement *))

void stmnt_dealloc(Statement *stmnt) {
  assert(COUNT_STMNT == STMNT_SIZE_OF_DEALLOC_FUNCS);
  if (stmnt == NULL)
    return;
  stmnt_dealloc_funcs[stmnt->type](stmnt);
  free(stmnt);
}

void (*stmnt_print_funcs[])(Statement *, FILE *, size_t) = {
  [ASSIGNMENT_STMNT] = stmnt_print_assignment_ident,
  [DECLARATION_STMNT] = stmnt_print_declaration_ident,
  [BLOCK_STMNT] = stmnt_print_block_ident,
  [IF_ELSE_STMNT] = stmnt_print_if_else_ident,
  [WHILE_STMNT] = stmnt_print_while_ident,
  [RETURN_STMNT] = stmnt_print_return_ident,
};

#define STMNT_SIZE_OF_PRINT_FUNCS sizeof(stmnt_print_funcs)/sizeof(void (*)(Statement *, FILE *))

void stmnt_print(Statement *stmnt, FILE *file) {
  assert(COUNT_STMNT == STMNT_SIZE_OF_PRINT_FUNCS);
  stmnt_print_funcs[stmnt->type](stmnt, file, 0);
}

void stmnt_print_ident(Statement *stmnt, FILE *file, size_t ident) {
  assert(COUNT_STMNT == STMNT_SIZE_OF_PRINT_FUNCS);
  stmnt_print_funcs[stmnt->type](stmnt, file, ident);
}

