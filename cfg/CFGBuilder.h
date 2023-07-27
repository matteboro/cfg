#pragma once

#include "../prsr/prgrm.h"
#include "CFG.h"

CFG *CFGBuilder_Build(ASTProgram *program);





CFG *CFGBuilder_Build(ASTProgram *program) {
  assert(program != NULL);
  StructDeclarationList *structs = program->struct_declarations;
  FunctionDeclarationList *functions = program->func_declarations;
  Statement *global_statements = program->global_statement;

  assert(strct_decl_list_size(structs) == 0);
  assert(func_decl_list_size(functions) == 0);
  assert(stmnt_is_block(global_statements));

  StatementList *statements = stmnt_block_get_body(global_statements);
  GlobalVariablesTable *var_table = GlobalVariablesTable_Create();

  FOR_EACH(StatementList, stmnt_it, statements) {
    stmnt_print(stmnt_it->node, stdout);
    fprintf(stdout, "\n");
  }
  GlobalVariablesTable_Print(var_table, stdout);

  GlobalVariablesTable_Destroy(var_table);

  return NULL;
}