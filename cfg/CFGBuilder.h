#pragma once

#include "../prsr/prgrm.h"
#include "CFG.h"

CFG *CFGBuilder_Build(ASTProgram *program);

// IMPLEMENTATION

CFGOperand *Operand_To_CFGOperand(Operand *operand, GlobalVariablesTable *var_table) {

  CFGOperand *cfg_operand = NULL;

  if (oprnd_is_integer(operand)) {
    int value = *oprnd_integer_get_integer(operand);
    Integer integer = Integer_Create(value);
    Literal *literal = Literal_Create_Integer_Init(integer);
    cfg_operand = CFGOperand_Create_Literal(literal);
  } 
  else if (oprnd_is_string(operand)) {
    char* cstring = oprnd_string_get_string(operand);
    char* cstring_copy = (char *) malloc(sizeof(char)*strlen(cstring)+1);
    strcpy(cstring_copy, cstring);
    String string = String_Create(cstring_copy);
    Literal *literal = Literal_Create_String_Init(string);
    cfg_operand = CFGOperand_Create_Literal(literal);
  }
  else if (oprnd_is_object_deref(operand)) {
    ObjectDerefList *obj_derefs = oprnd_object_deref_get_derefs(operand);
    
    assert(obj_drf_list_size(obj_derefs) == 1);

    ObjectDeref *obj_drf = obj_drf_list_get_at(obj_derefs, 0);
    Identifier *name = obj_drf->name;
    Variable deref_var = GlobalVariablesTable_GetFromName(var_table, name);
    assert(!Variable_IsNull(deref_var));
    AccessOperation *access_op = AccessOperation_Create_Variable_Access(deref_var);
    cfg_operand = CFGOperand_Create_AccessOperation(access_op);
  }
  else if (oprnd_is_funccall(operand)) {
    UNREACHABLE();
  }
  else {
    UNREACHABLE();
  }

  assert(cfg_operand != NULL);
  return cfg_operand;
}

CFGExpression *Expression_To_CFGExpression(Expression *expr, GlobalVariablesTable *var_table) {

  CFGExpression *cfg_expr = NULL;

  if (expr_is_binary_expression(expr)) {
    Expression *left_expr = expr_binary_expression_get_left(expr);
    Expression *right_expr = expr_binary_expression_get_right(expr);
    OperationType operation = expr_binary_expression_get_operation(expr);

    assert(expr_is_operand(left_expr));
    assert(expr_is_operand(right_expr));

    CFGOperand *left  = Operand_To_CFGOperand(expr_operand_expression_get_operand(left_expr),  var_table);
    CFGOperand *right = Operand_To_CFGOperand(expr_operand_expression_get_operand(right_expr), var_table);
    cfg_expr = CFGExpression_Create_BinaryExpression(left, right, operation);
  } 
  else if (expr_is_unary_expression(expr)) {
    Expression *operand_expr = expr_unary_expression_get_operand(expr);
    OperationType operation = expr_binary_expression_get_operation(expr);

    assert(expr_is_operand(operand_expr));

    CFGOperand *operand = Operand_To_CFGOperand(expr_operand_expression_get_operand(operand_expr), var_table);
    cfg_expr = CFGExpression_Create_UnaryExpression(operand, operation);
  }
  else if (expr_is_operand(expr)) {
    CFGOperand *operand = Operand_To_CFGOperand(expr_operand_expression_get_operand(expr), var_table);
    cfg_expr = CFGExpression_Create_OperandExpression(operand);
  }
  else {
    UNREACHABLE();
  }

  assert(cfg_expr != NULL);
  return cfg_expr;
}

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
  VariableIndex curr_var_idx = 1;

  CFGStatementList *cfg_statements = CFGStatement_list_create_empty();

  FOR_EACH(StatementList, stmnt_it, statements) {
    
    Statement *stmnt = stmnt_it->node;
    assert(stmnt_is_assignment(stmnt) || stmnt_is_declaration(stmnt));

    if (stmnt_is_assignment(stmnt_it->node)) {
      AssignableElement *assgnbl = stmnt_assignment_get_assgnbl(stmnt);
      ObjectDerefList *obj_derefs = assgnbl->obj_derefs;

      assert(obj_drf_list_size(obj_derefs) == 1);

      ObjectDeref *obj_drf = obj_drf_list_get_at(obj_derefs, 0);
      Identifier *name = obj_drf->name;

      Variable assigned_var = GlobalVariablesTable_GetFromName(var_table, name);
      assert(!Variable_IsNull(assigned_var));
      AccessOperation *access_op = AccessOperation_Create_Variable_Access(assigned_var);

      CFGExpression *cfg_value_expr = Expression_To_CFGExpression(stmnt_assignment_get_value(stmnt), var_table);
      assert(cfg_value_expr != NULL);
      
      CFGStatement *cfg_stmnt = CFGStatement_Create_Assignment(access_op, cfg_value_expr);
      CFGStatement_list_append(cfg_statements, cfg_stmnt);

    } 
    else if (stmnt_is_declaration(stmnt_it->node)) {
      NameTypeBinding *nt_bind = stmnt_declaration_get_nt_bind(stmnt);
      Identifier *name = nt_bind->name;
      Type *type = nt_bind->type;

      assert(type_is_basic(type));

      Variable *decl_var = Variable_Create_Pointer(
        curr_var_idx,
        idf_copy_identifier(name),
        type_copy(type));
      
      GlobalVariablesTable_Add_Variable(var_table, decl_var);

      CFGStatement *cfg_stmnt = CFGStatement_Create_Declaration(*decl_var);
      CFGStatement_list_append(cfg_statements, cfg_stmnt);

      if (stmnt_declaration_has_init_values(stmnt)) {
        ExpressionList *init_values = stmnt_declaration_get_init_values(stmnt);
        assert(expr_list_size(init_values) == 1);

        Expression *init_value = expr_list_get_at(init_values, 0);
        CFGExpression *cfg_init_value_expr = Expression_To_CFGExpression(init_value, var_table);
        assert(cfg_init_value_expr != NULL);

        AccessOperation *access_op = AccessOperation_Create_Variable_Access(*decl_var);
        CFGStatement *cfg_init_assgnmt = CFGStatement_Create_Assignment(access_op, cfg_init_value_expr);
        CFGStatement_list_append(cfg_statements, cfg_init_assgnmt);
      }
    }

    stmnt_print(stmnt_it->node, stdout);
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "\n");
  FOR_EACH(CFGStatementList, cfg_stmnt_it, cfg_statements) {
    CFGStatement_Print(cfg_stmnt_it->node, stdout);
    fprintf(stdout, "\n");
  }

  CFGStatement_list_dealloc(cfg_statements);

  fprintf(stdout, "\n");
  GlobalVariablesTable_Print(var_table, stdout);
  GlobalVariablesTable_Destroy(var_table);

  return NULL;
}