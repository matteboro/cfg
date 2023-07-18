#pragma once

#include "../expr/expr.h"
#include "assgnbl.h"
#include "../expr/funccall.h"


// FORWARD DEFINITIONS
struct FunctionDeclaration_s;
typedef struct FunctionDeclaration_s FunctionDeclaration;
void func_decl_dealloc(FunctionDeclaration *func);
void func_decl_print_signature(FunctionDeclaration *func, FILE *file);

#define STMNT_ALLOC_PAYLOAD(type) type *payload = (type *)malloc(sizeof(type))
#define STMNT_CAST_PAYLOAD(type, obj) type *payload = (type *)obj->payload
#define STMNT_ALLOC(type) (type *)malloc(sizeof(type))
#define print_spaces(n, file) {for (size_t spaces_counter=0; spaces_counter<n; ++spaces_counter) fprintf(file, " ");}
#define STMNT_GETTER(stmnt_prefix, obj_type, obj_name, payload_type, stmnt_type)  \
obj_type *stmnt_## stmnt_prefix ## _get_ ## obj_name  (Statement* stmnt) {        \
  assert(stmnt->type == stmnt_type);                                              \
  STMNT_CAST_PAYLOAD(payload_type, stmnt);                                        \
  return payload->obj_name;                                                       \
} 

#define STMNT_SETTER(stmnt_prefix, obj_type, obj_name, payload_type, stmnt_type)        \
void stmnt_## stmnt_prefix ## _set_ ## obj_name  (Statement* stmnt, obj_type *obj) {    \
  assert(stmnt->type == stmnt_type);                                                    \
  STMNT_CAST_PAYLOAD(payload_type, stmnt);                                              \
  payload->obj_name = obj;                                                              \
  return;                                                                               \
} 

#define STMNT_SET_GET(stmnt_prefix, obj_type, obj_name, payload_type, stmnt_type) \
  STMNT_SETTER(stmnt_prefix, obj_type, obj_name, payload_type, stmnt_type)        \
  STMNT_GETTER(stmnt_prefix, obj_type, obj_name, payload_type, stmnt_type)        \

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
  FUNCCALL_STMNT,
  // define above here
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

STMNT_GETTER(assignment, AssignableElement, assgnbl, AssignmentPayload, ASSIGNMENT_STMNT)
STMNT_GETTER(assignment, Expression, value, AssignmentPayload, ASSIGNMENT_STMNT)

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

STMNT_GETTER(declaration, NameTypeBinding, nt_bind, DeclarationPayload, DECLARATION_STMNT)
STMNT_GETTER(declaration, ExpressionList, init_values, DeclarationPayload, DECLARATION_STMNT)

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

STMNT_GETTER(if_else, Expression, condition, IfElsePayload, IF_ELSE_STMNT)
STMNT_GETTER(if_else, Statement, if_body, IfElsePayload, IF_ELSE_STMNT)
STMNT_GETTER(if_else, Statement, else_body, IfElsePayload, IF_ELSE_STMNT)

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

  if (payload->else_body == NULL)
    return;

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

STMNT_GETTER(while, Expression, condition, WhilePayload, WHILE_STMNT)
STMNT_GETTER(while, Statement, body, WhilePayload, WHILE_STMNT)

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

// TODO: the return statement should also include the FunctionDeclarations which is part of
//   that is how I want to do it: 
//     I want a method 
//       void stmnt_set_funtion_declaration_to_return(Statement *s, FunctionDeclaration *f) 
//     that receive a statement and:
//       - if the statementnis a return, set the attribute func_decl to f;
//       - if is a block statement calls stmnt_set_funtion_declaration_to_return(s, f) to all 
//         the statements inside;

struct ReturnPayload_s;
typedef struct ReturnPayload_s ReturnPayload;

Statement *stmnt_create_return(Expression *);
void stmnt_print_return_ident(Statement *, FILE *, size_t);
void stmnt_dealloc_return(Statement *);

struct ReturnPayload_s {
  Expression *ret_value;
  FunctionDeclaration *func_decl;
};

STMNT_SETTER(return, Expression, ret_value, ReturnPayload, RETURN_STMNT)
STMNT_SETTER(return, FunctionDeclaration, func_decl, ReturnPayload, RETURN_STMNT)

STMNT_GETTER(return, Expression, ret_value, ReturnPayload, RETURN_STMNT)
STMNT_GETTER(return, FunctionDeclaration, func_decl, ReturnPayload, RETURN_STMNT)


Statement *stmnt_create_return(Expression *ret_value) {
  STMNT_ALLOC_PAYLOAD(ReturnPayload);
  payload->ret_value = ret_value;
  payload->func_decl = NULL;
  return stmnt_create(payload, RETURN_STMNT);
}

void stmnt_print_return_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(ReturnPayload, stmnt);
  print_spaces(ident, file);
  fprintf(file, "return ");
  expr_print_expression(payload->ret_value, file);
  fprintf(file, " <-- ");
  func_decl_print_signature(payload->func_decl, file);
}

void stmnt_dealloc_return(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(ReturnPayload, stmnt);
  expr_dealloc_expression(payload->ret_value);
  // func_decl_dealloc(payload->func_decl);
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

STMNT_GETTER(block, StatementList, body, BlockPayload, BLOCK_STMNT)

Statement *stmnt_create_block(StatementList *body) {
  STMNT_ALLOC_PAYLOAD(BlockPayload);
  payload->body = body;
  return stmnt_create(payload, BLOCK_STMNT);
}

void stmnt_print_block_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(BlockPayload, stmnt);
  FOR_EACH(StatementList, stmnt_it, payload->body) {
    if (stmnt_it->node->type == BLOCK_STMNT) {
      print_spaces(ident, file);
      fprintf(file, "{\n");
      stmnt_print_ident(stmnt_it->node, file, ident+2);
      fprintf(file, "\n");
      print_spaces(ident, file);
      fprintf(file, "}");
    } else {
      stmnt_print_ident(stmnt_it->node, file, ident);
    }
    if (stmnt_it->next != NULL)
      fprintf(file, "\n");
  }
}

void stmnt_dealloc_block(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(BlockPayload, stmnt);
  stmnt_list_dealloc(payload->body);
  free(payload);
}

// FUNCCALL STATEMENT

struct FunctionCallPayload_s;
typedef struct FunctionCallPayload_s FunctionCallPayload;

Statement *stmnt_create_funccall(FunctionCall *);
void stmnt_print_funccall_ident(Statement *, FILE *, size_t);
void stmnt_dealloc_funccall(Statement *);

struct FunctionCallPayload_s {
  FunctionCall *funccall;
};

STMNT_GETTER(funccall, FunctionCall, funccall, FunctionCallPayload, FUNCCALL_STMNT)

Statement *stmnt_create_funccall(FunctionCall *funccall) {
  STMNT_ALLOC_PAYLOAD(FunctionCallPayload);
  payload->funccall = funccall;
  return stmnt_create(payload, FUNCCALL_STMNT);
}

void stmnt_print_funccall_ident(Statement *stmnt, FILE *file, size_t ident) {
  STMNT_CAST_PAYLOAD(FunctionCallPayload, stmnt);
  print_spaces(ident, file);
  funccall_print(payload->funccall, file);
}

void stmnt_dealloc_funccall(Statement *stmnt) {
  STMNT_CAST_PAYLOAD(FunctionCallPayload, stmnt);
  funccall_dealloc(payload->funccall);
  free(payload);
}

// GENERAL DEFINITIONS

void stmnt_set_funtion_declaration_to_return(Statement *stmnt, FunctionDeclaration *func_decl) {
  if (stmnt->type == RETURN_STMNT) {
    stmnt_return_set_func_decl(stmnt, func_decl);
  } else if (stmnt->type == BLOCK_STMNT) {
    FOR_EACH(StatementList, stmnt_it, stmnt_block_get_body(stmnt)) {
      stmnt_set_funtion_declaration_to_return(stmnt_it->node, func_decl);
    }
  }
  return;
}

#define STMNT_PRINT_SIGN   void (*)(Statement *, FILE *, size_t)
#define STMNT_DEALLOC_SIGN void (*)(Statement *)

enum {
  STMNT_PRINT_FUNC   = 0,
  STMNT_DEALLOC_FUNC = 1,
  STMNT_COUNT_FUNC,
};

void *stmnt_funcs_map[][STMNT_COUNT_FUNC] = {
  [ASSIGNMENT_STMNT]   = {stmnt_print_assignment_ident,   stmnt_dealloc_assignment},
  [DECLARATION_STMNT]  = {stmnt_print_declaration_ident,  stmnt_dealloc_declaration},
  [BLOCK_STMNT]        = {stmnt_print_block_ident,        stmnt_dealloc_block},
  [IF_ELSE_STMNT]      = {stmnt_print_if_else_ident,      stmnt_dealloc_if_else},
  [WHILE_STMNT]        = {stmnt_print_while_ident,        stmnt_dealloc_while},
  [RETURN_STMNT]       = {stmnt_print_return_ident,       stmnt_dealloc_return},
  [FUNCCALL_STMNT]     = {stmnt_print_funccall_ident,     stmnt_dealloc_funccall},
};

#define STMNT_SIZE_OF_FUNCS_MAP (sizeof(stmnt_funcs_map)/sizeof(void *))/STMNT_COUNT_FUNC

void stmnt_dealloc(Statement *stmnt) {
  assert(COUNT_STMNT == STMNT_SIZE_OF_FUNCS_MAP);
  if (stmnt == NULL)
    return;
  ((STMNT_DEALLOC_SIGN)stmnt_funcs_map[stmnt->type][STMNT_DEALLOC_FUNC])(stmnt);
  free(stmnt);
}

void stmnt_print_ident(Statement *stmnt, FILE *file, size_t ident) {
  assert(COUNT_STMNT == STMNT_SIZE_OF_FUNCS_MAP);
  ((STMNT_PRINT_SIGN)stmnt_funcs_map[stmnt->type][STMNT_PRINT_FUNC])(stmnt, file, ident);
}

void stmnt_print(Statement *stmnt, FILE *file) {
  stmnt_print_ident(stmnt, file, 0);
}
