#ifndef PRMT_HEADER
#define PRMT_HEADER

#include "expr_interface.h"
#include "idf.h"
#include "nt_bind.h"
#include "funccall.h"

#define PRMT_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }


// PARAMETER

// TODO: implment better the data part of Parameter
//       create a data structure for each kind of Parameter,
//       but I think the only used kind are expression and function
//       declarations ones.

typedef enum {
  IDENTIFIER_PARAM,
  INTEGER_PARAM,
  EXPRESSION_PARAM,
  FUNCCALL_PARAM,
  FUNCDEC_PARAM,
} ParameterType;

typedef struct {
  ParameterType type;
  void* param;
} Parameter;

//// FORWARD DECLARATIONS OF FUNCCALL PARAMETER

void prmt_dealloc_funccall_param(Parameter *param);
void prmt_print_funccall_param(Parameter *param, FILE *file);

//// CREATE

Parameter *prmt_create_identifer_param(Identifier *id) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = IDENTIFIER_PARAM;
  param->param = id;
  return param;
}

Parameter *prmt_create_expression_param(Expression *expr) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = EXPRESSION_PARAM;
  param->param = expr;
  return param;
}

Parameter *prmt_create_integer_param(int i) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = INTEGER_PARAM;
  param->param = malloc(sizeof(int));
  *((int *)param->param) = i;
  return param;
}

Parameter *prmt_create_funcdec_param(NameTypeBinding *nt_bind) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = FUNCDEC_PARAM;
  param->param = malloc(sizeof(NameTypeBinding));
  param->param = nt_bind;
  return param;
}

//// DEALLOC

void prmt_dealloc_identifer_param(Parameter *param) {
  Identifier *id = (Identifier *) param->param;
  idf_dealloc_identifier(id);
}

void prmt_dealloc_expression_param(Parameter *param) {
  Expression *expr = (Expression *) param->param;
  expr_dealloc_expression(expr);
}

void prmt_dealloc_integer_param(Parameter *param) {
  free(param->param);
}

void prmt_dealloc_funcdec_param(Parameter *param) {
  NameTypeBinding *nt_bind = (NameTypeBinding *) param->param;
  nt_bind_dealloc(nt_bind);
}

void prmt_dealloc_param(Parameter *param) {
  if (param == NULL)
    return;
  switch (param->type) {
  case IDENTIFIER_PARAM:
    prmt_dealloc_identifer_param(param); break;
  case EXPRESSION_PARAM:
    prmt_dealloc_expression_param(param); break;
  case INTEGER_PARAM:
    prmt_dealloc_integer_param(param); break;
  case FUNCCALL_PARAM:
    prmt_dealloc_funccall_param(param); break;
  case FUNCDEC_PARAM:
    prmt_dealloc_funcdec_param(param); break;
  default:
    PRMT_ERROR();
  }
  free(param);
}

//// PRINT

void prmt_print_identifer_param(Parameter *param, FILE *file) {
  Identifier *id = (Identifier *) param->param;
  idf_print_identifier(id, file);
}

void prmt_print_expression_param(Parameter *param, FILE *file) {
  Expression *expr = (Expression *) param->param;
  expr_print_expression(expr, file);
}

void prmt_print_integer_param(Parameter *param, FILE *file) {
  fprintf(file, "%d", *((int *)param->param));
}

void prmt_print_funcdec_param(Parameter *param, FILE *file) {
  NameTypeBinding *nt_bind = (NameTypeBinding *) param->param;
  nt_bind_print(nt_bind, file);
}

void prmt_print_param(Parameter *param, FILE *file) {
  if_null_print(param, file);
  switch (param->type) {
  case IDENTIFIER_PARAM:
    prmt_print_identifer_param(param, file); break;
  case EXPRESSION_PARAM:
    prmt_print_expression_param(param, file); break;
  case INTEGER_PARAM:
    prmt_print_integer_param(param, file); break;
  case FUNCCALL_PARAM:
    prmt_print_funccall_param(param, file); break;
  case FUNCDEC_PARAM:
    prmt_print_funcdec_param(param, file); break;
  default:
    PRMT_ERROR();
  }
}

// END PARAMETER

//// FUNCALL PARAMETER

Parameter *prmt_create_funccall_param(FunctionCall *func_call) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = FUNCCALL_PARAM;
  param->param = func_call;
  return param;
}

void prmt_dealloc_funccall_param(Parameter *param) {
  FunctionCall *func_call = (FunctionCall *) param->param;
  funccall_dealloc(func_call);
}

void prmt_print_funccall_param(Parameter *param, FILE *file) {
  FunctionCall *func_call = (FunctionCall *) param->param;
  funccall_print(func_call, file);
}

// PARAMTER LIST

typedef struct ParameterList_s{
  struct ParameterList_s *next;
  Parameter *param;
} ParameterList;

//// CREATE EMPTY

ParameterList *prmt_list_create_empty() {
  ParameterList *list = (ParameterList *)malloc(sizeof(ParameterList));
  list->next = NULL;
  list->param = NULL;
  return list;
  // return NULL;
}

ParameterList *prmt_list_create(Parameter *param) {
  ParameterList *list = (ParameterList *)malloc(sizeof(ParameterList));
  list->next = NULL;
  list->param = param;
  return list;
}

//// APPEND

void prmt_list_append(ParameterList *list, Parameter *param) {
  if (list->next == NULL) {
    if (list->param == NULL) {
      list->param = param;
    } else {
      list->next = prmt_list_create(param);
    }
    return;
  }

  ParameterList *n = list->next;
  while(n->next != NULL)
    n = n->next;

  n->next = prmt_list_create(param);
  return;
}

//// DEALLOC

void prmt_list_dealloc(ParameterList *list) {
  if (list == NULL)
    return;
  if (list->next != NULL)
    prmt_list_dealloc(list->next);
  
  if (list->param != NULL)
    prmt_dealloc_param(list->param);
  
  free(list);
}

//// SIZE

size_t prmt_list_size(ParameterList *list) {
  if (list->next != NULL)
    return prmt_list_size(list->next) + 1;
  
  if (list->param != NULL)
    return 1;
  
  return 0;
}

//// GET AT

Parameter *prmt_list_get_at(ParameterList *list, size_t index) {
  if (list->next == NULL && list->param == NULL && index == 0)
    return NULL;
  assert(index < prmt_list_size(list));
  ParameterList *n = list;
  for (size_t i=0; i<index; ++i)
    n = n->next;
  return n->param;
}

//// PRINT

void prmt_list_print(ParameterList *list, FILE *file) {
  if_null_print(list, file);
  if (list->param != NULL)
    prmt_print_param(list->param, file);
  
  if (list->next != NULL){
    fprintf(file, ", ");
    prmt_list_print(list->next, file);
  }
}

// END PARAMETER LIST

#endif // end PRMT_HEADER