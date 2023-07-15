#pragma once

#include <assert.h>
#include "strct_decl.h"
#include "stmnt.h"
#include "func_decl.h"


struct ASTProgram_s;
typedef struct ASTProgram_s ASTProgram;

ASTProgram *prgrm_create(StructDeclarationList *, FunctionDeclarationList *, Statement *);
void prgrm_print(ASTProgram *, FILE *);
void prgrm_dealloc(ASTProgram *);

struct ASTProgram_s {
  FunctionDeclarationList *func_declarations;
  StructDeclarationList *struct_declarations;
  Statement *global_statement;
};

ASTProgram *prgrm_create(
  StructDeclarationList *struct_declarations, 
  FunctionDeclarationList *func_declarations, 
  Statement *global_statement) {
  ASTProgram *prgrm = (ASTProgram *) malloc(sizeof(ASTProgram));
  prgrm->func_declarations = func_declarations;
  prgrm->struct_declarations = struct_declarations;
  prgrm->global_statement = global_statement;
  return prgrm;
}

void prgrm_print(ASTProgram *prgrm, FILE *file) {

  fprintf(file, "Program: {\n");

  //functions
  fprintf(file, "  Functions: [\n");
  func_decl_list_print_ident(prgrm->func_declarations, file, 4);
  fprintf(file, "\n");
  fprintf(file, "  ]\n");

  //structs
  fprintf(file, "  Data Structs: {\n");
  strct_decl_list_print_ident(prgrm->struct_declarations, file, 4);
  fprintf(file, "\n");
  fprintf(file, "  }\n");

  //statements
  fprintf(file, "  Global Statements: {\n");
  stmnt_print_ident(prgrm->global_statement, file, 4);
  fprintf(file, "\n");
  fprintf(file, "  }\n");
  fprintf(file, "}");
}

void prgrm_dealloc(ASTProgram *prgrm) {
  func_decl_list_dealloc(prgrm->func_declarations);
  strct_decl_list_dealloc(prgrm->struct_declarations);
  stmnt_dealloc(prgrm->global_statement);
  free(prgrm);
}

