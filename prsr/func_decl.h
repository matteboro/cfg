#pragma once

#include "../expr/prmt.h"
#include "stmnt.h"

struct FunctionDeclaration_s;
typedef struct FunctionDeclaration_s FunctionDeclaration;

FunctionDeclaration *func_decl_create(Identifier *name, Type *ret_type, ParameterList *params, Statement *body);
void func_decl_print(FunctionDeclaration *func, FILE *file);
void func_decl_print_ident(FunctionDeclaration *func, FILE *file, size_t ident);
void func_decl_dealloc(FunctionDeclaration *func);

struct FunctionDeclaration_s {
  Identifier *name;
  Type *ret_type;
  ParameterList *params;
  Statement *body;
};

FunctionDeclaration *func_decl_create(Identifier *name, Type *ret_type, ParameterList *params, Statement *body) {
  FunctionDeclaration *func = (FunctionDeclaration *) malloc(sizeof(FunctionDeclaration));
  func->name = name;
  func->ret_type = ret_type;
  func->params = params;
  func->body = body;
  stmnt_set_funtion_declaration_to_return(body, func);
  return func;
}

void func_decl_print_signature(FunctionDeclaration *func, FILE *file) {
  type_print(func->ret_type, file);

  fprintf(file, " :: ");
  idf_print_identifier(func->name, file);

  fprintf(file, "(");
  prmt_list_print(func->params, file);
  fprintf(file, ")");
}

void func_decl_print(FunctionDeclaration *func, FILE *file) {
  func_decl_print_ident(func, file, 0);
}

void func_decl_print_ident(FunctionDeclaration *func, FILE *file, size_t ident) {
  print_spaces(ident, file);
  type_print(func->ret_type, file);

  fprintf(file, " :: ");
  idf_print_identifier(func->name, file);

  fprintf(file, "(");
  prmt_list_print(func->params, file);
  fprintf(file, ") {\n");

  stmnt_print_ident(func->body, file, ident+2);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "}");
}

void func_decl_dealloc(FunctionDeclaration *func) {
  if (func == NULL)
    return;
  idf_dealloc_identifier(func->name);
  type_dealloc(func->ret_type);
  prmt_list_dealloc(func->params);
  stmnt_dealloc(func->body);
  free(func);
}

DEFAULT_LIST_IMPLEMENTATION(FunctionDeclaration)                    
DEFAULT_LIST_CREATE_EMPTY(func_decl, FunctionDeclaration)              
DEFAULT_LIST_CREATE(func_decl, FunctionDeclaration)                    
DEFAULT_LIST_APPEND(func_decl, FunctionDeclaration)                    
DEFAULT_LIST_DEALLOC(func_decl, FunctionDeclaration, func_decl_dealloc)     
DEFAULT_LIST_SIZE(func_decl, FunctionDeclaration)                      
DEFAULT_LIST_GET_AT(func_decl, FunctionDeclaration)                    

void func_decl_list_print(FunctionDeclarationList *list, FILE *file) {
  if_null_print(list, file);                                    
  if (list->node != NULL)                                       
    func_decl_print(list->node, file);                               
  if (list->next != NULL){                                      
    fprintf(file, ", \n");                                        
    func_decl_list_print(list->next, file);                      
  }   
}

void func_decl_list_print_ident(FunctionDeclarationList *list, FILE *file, size_t ident) {
  if_null_print(list, file);                                    
  if (list->node != NULL)                                       
    func_decl_print_ident(list->node, file, ident);                               
  if (list->next != NULL){                                      
    fprintf(file, ", \n");                                        
    func_decl_list_print_ident(list->next, file, ident);                      
  }   
}