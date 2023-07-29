#pragma once

#include "../prsr/prgrm.h"
#include "CFG.h"
#include "BasicBlock.h"

CFG *CFGBuilder_Build(ASTProgram *program);

// IMPLEMENTATION

static VariableIndex curr_var_idx = 1;

#define next_var_idx() curr_var_idx++


AccessOperation *ObjectDerefList_To_AccessOperation(ObjectDerefList *obj_derefs, GlobalVariablesTable *var_table) {
  assert(obj_derefs != NULL);
  assert(obj_drf_list_size(obj_derefs) != 0);
  assert(var_table != NULL);

  // fprintf(stdout, "started ObjectDerefList_To_AccessOperation\n");

  // get variable of first element in ObjectDerefList
  ObjectDeref *obj_drf = obj_drf_list_get_at(obj_derefs, 0);
  Identifier *name = obj_drf->name;
  Variable deref_var = GlobalVariablesTable_GetFromName(var_table, name);
  assert(!Variable_IsNull(deref_var));

  if (obj_drf_list_size(obj_derefs) == 1) {
    return AccessOperation_Create_Variable_Access(deref_var);
  }

  // fprintf(stdout, "obj derefs has more than 1 element\n");

  ObjectDerefList *curr_deref = obj_derefs->next;
  ObjectDeref *deref = curr_deref->node;
  Type *curr_deref_type = obj_drf_get_real_type(deref);
  size_t index = 0;
  while (!type_is_pointer(curr_deref_type) && (curr_deref != NULL && curr_deref->node != NULL)) {
    // fprintf(stdout, "starting while loop\n");
    deref = curr_deref->node;
    curr_deref_type = obj_drf_get_real_type(deref);

    // fprintf(stdout, "  curr deref: "); obj_drf_print(deref, stdout); fprintf(stdout, "\n");
    // fprintf(stdout, "  curr deref type: "); type_print(curr_deref_type, stdout); fprintf(stdout, "\n");

    Attribute *curr_attribute = obj_drf_get_attribute(deref);
    assert(attrb_relative_position_is_valid(curr_attribute));
    size_t attribute_offset = (size_t) attrb_get_relative_position(curr_attribute);

    // fprintf(stdout, "  curr attribute: "); attrb_print(curr_attribute, stdout); fprintf(stdout, "\n");
    // fprintf(stdout, "  curr attribute offset: %lu\n", attribute_offset);

    // if it is an array non pointer offset we have to calculate the offset
    // contrinution of the index
    if (type_is_array(curr_deref_type)) {
      Expression *index_expr = obj_drf_array_get_index(deref);
      
      // for the moment we only process integer indexes
      assert(expr_is_operand(index_expr));
      Operand *index_operand = expr_operand_expression_get_operand(index_expr);
      assert(oprnd_is_integer(index_operand));
      size_t array_index = (size_t)(*oprnd_integer_get_integer(index_operand));

      // if sub-type is a struct we have to multiply the index by the size
      // of the struct 
      size_t struct_mult_factor = 1;
      Type *sub_type = type_array_get_type(curr_deref_type);
      if (type_is_struct(sub_type)) {
        StructDeclaration *struct_decl = type_struct_get_struct_decl(sub_type);
        struct_mult_factor = strct_decl_total_number_of_attributes(struct_decl);
      }

      index += array_index * struct_mult_factor;
    } 

    index += attribute_offset;

    curr_deref = curr_deref->next;
    assert(!type_is_pointer(curr_deref_type));

    // fprintf(stdout, "end while loop\n");
  }
  
  // fprintf(stdout, "exited while loop\n");

  CFGOperand *op_idx = CFGOperand_Create_Literal(Literal_Create_Integer_Init(Integer_Create(index)));
  return AccessOperation_Create_Indexed_Variable_Access(deref_var, op_idx);
}

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
    AccessOperation *access_op = ObjectDerefList_To_AccessOperation(obj_derefs, var_table);
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

CFGExpression *BasicExpression_To_CFGExpression(Expression *expr, GlobalVariablesTable *var_table) {

  assert(expr_children_are_operands(expr));
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

typedef struct {
  CFGStatementList *cfg_statements;
  CFGOperand *cfg_operand;
} CFGStatementList_X_CFGOperand;

CFGStatementList_X_CFGOperand
  Expression_To_CFGStatementList_X_CFGOperand(Expression *expr, GlobalVariablesTable *var_table) {

  assert(expr != NULL);
  assert(var_table != NULL);

  if (expr_is_binary_expression(expr)) {
    Expression *left_sub_expr = expr_binary_expression_get_left(expr);
    Expression *right_sub_expr = expr_binary_expression_get_right(expr);

    CFGStatementList_X_CFGOperand left_sub_pair = Expression_To_CFGStatementList_X_CFGOperand(left_sub_expr, var_table);
    CFGStatementList_X_CFGOperand right_sub_pair = Expression_To_CFGStatementList_X_CFGOperand(right_sub_expr, var_table);

    Type *expr_type = expr_get_real_type(expr); 
    assert(expr_type != NULL);

    OperationType operation = expr_binary_expression_get_operation(expr);

    Variable tmp_var = GlobalVariablesTable_AddNextTempVariable(var_table, next_var_idx(), type_copy(expr_type));
    AccessOperation *tmp_access_op = AccessOperation_Create_Variable_Access(tmp_var);
    CFGExpression *tmp_value_expr = 
      CFGExpression_Create_BinaryExpression(left_sub_pair.cfg_operand, right_sub_pair.cfg_operand, operation);
    CFGStatement *tmp_assignment = CFGStatement_Create_Assignment(tmp_access_op, tmp_value_expr);
    CFGStatement *tmp_declaration = CFGStatement_Create_Declaration(tmp_var);

    CFGStatementList *cfg_statements = CFGStatementList_Concat(left_sub_pair.cfg_statements, right_sub_pair.cfg_statements);

    CFGStatementList_Append(cfg_statements, tmp_declaration);
    CFGStatementList_Append(cfg_statements, tmp_assignment);

    AccessOperation *tmp_future_access = AccessOperation_Create_Variable_Access(tmp_var);
    CFGOperand *tmp_future_operand = CFGOperand_Create_AccessOperation(tmp_future_access);
    
    CFGStatementList_X_CFGOperand pair = {
      .cfg_operand = tmp_future_operand,
      .cfg_statements = cfg_statements,
    };
    return pair;
  }
  else if (expr_is_unary_expression(expr)) {
    Expression *sub_expr = expr_unary_expression_get_operand(expr);

    CFGStatementList_X_CFGOperand sub_pair = Expression_To_CFGStatementList_X_CFGOperand(sub_expr, var_table);

    Type *expr_type = expr_get_real_type(expr); 
    assert(expr_type != NULL);

    OperationType operation = expr_unary_expression_get_operation(expr);

    Variable tmp_var = GlobalVariablesTable_AddNextTempVariable(var_table, next_var_idx(), type_copy(expr_type));
    AccessOperation *tmp_access_op = AccessOperation_Create_Variable_Access(tmp_var);
    CFGExpression *tmp_value_expr = CFGExpression_Create_UnaryExpression(sub_pair.cfg_operand, operation);
    CFGStatement *tmp_assignment = CFGStatement_Create_Assignment(tmp_access_op, tmp_value_expr);
    CFGStatement *tmp_declaration = CFGStatement_Create_Declaration(tmp_var);

    CFGStatementList *cfg_statements = sub_pair.cfg_statements;
    CFGStatementList_Append(cfg_statements, tmp_declaration);
    CFGStatementList_Append(cfg_statements, tmp_assignment);

    AccessOperation *tmp_future_access = AccessOperation_Create_Variable_Access(tmp_var);
    CFGOperand *tmp_future_operand = CFGOperand_Create_AccessOperation(tmp_future_access);
    
    CFGStatementList_X_CFGOperand pair = {
      .cfg_operand = tmp_future_operand,
      .cfg_statements = cfg_statements,
    };
    return pair;
  }
  else if (expr_is_operand(expr)) {
    Operand *operand = expr_operand_expression_get_operand(expr);
    CFGOperand *cfg_operand = Operand_To_CFGOperand(operand, var_table);
    CFGStatementList_X_CFGOperand stmnt_oprnd_pair = {
      .cfg_operand = cfg_operand,
      .cfg_statements = CFGStatementList_CreateEmpty()
    };
    return stmnt_oprnd_pair;
  } 
  else if (expr_is_create_expression(expr)) {
    TODO();
  }

  UNREACHABLE();
  CFGStatementList_X_CFGOperand null_result = { NULL, NULL };
  return null_result;
}

typedef struct {
  CFGStatementList *cfg_statements;
  CFGExpression *cfg_expression;
} CFGStatementList_X_CFGExpression;

CFGStatementList_X_CFGExpression
  Expression_To_CFGStatementList_X_CFGExpression(Expression *expr, GlobalVariablesTable *var_table) {

  // base case
  if (expr_children_are_operands(expr)) {
    CFGExpression *cfg_exprexssion = BasicExpression_To_CFGExpression(expr, var_table);
    CFGStatementList_X_CFGExpression stmnt_expr_pair = {
      .cfg_expression = cfg_exprexssion,
      .cfg_statements = CFGStatementList_CreateEmpty()
    };
    return stmnt_expr_pair;
  }

  if (expr_is_binary_expression(expr)) {
    Expression *left_sub_expr = expr_binary_expression_get_left(expr);
    Expression *right_sub_expr = expr_binary_expression_get_right(expr);

    OperationType operation = expr_binary_expression_get_operation(expr);

    CFGStatementList_X_CFGOperand left_sub_pair = Expression_To_CFGStatementList_X_CFGOperand(left_sub_expr, var_table);
    CFGStatementList_X_CFGOperand right_sub_pair = Expression_To_CFGStatementList_X_CFGOperand(right_sub_expr, var_table);

    CFGExpression *cfg_expr = 
      CFGExpression_Create_BinaryExpression(left_sub_pair.cfg_operand, right_sub_pair.cfg_operand, operation);

    CFGStatementList *cfg_statements = CFGStatementList_Concat(left_sub_pair.cfg_statements, right_sub_pair.cfg_statements);

    CFGStatementList_X_CFGExpression pair = { .cfg_expression = cfg_expr, .cfg_statements = cfg_statements };
    return pair;
  } 
  else if(expr_is_unary_expression(expr)) {
    Expression *sub_expr = expr_unary_expression_get_operand(expr);

    OperationType operation = expr_unary_expression_get_operation(expr);

    CFGStatementList_X_CFGOperand sub_pair = Expression_To_CFGStatementList_X_CFGOperand(sub_expr, var_table);

    CFGExpression *cfg_expr = CFGExpression_Create_UnaryExpression(sub_pair.cfg_operand, operation);

    CFGStatementList *cfg_statements = sub_pair.cfg_statements;

    CFGStatementList_X_CFGExpression pair = { .cfg_expression = cfg_expr, .cfg_statements = cfg_statements };
    return pair;
  }
  else if(expr_is_create_expression(expr)) {
    assert(False);
  } 
  
  UNREACHABLE();
  CFGStatementList_X_CFGExpression null_result = { NULL, NULL };
  return null_result;
}

CFGStatementList *AssignmentStatement_To_CFGStatementList(Statement *stmnt, GlobalVariablesTable *var_table) {
  
  assert(stmnt_is_assignment(stmnt));
  assert(var_table != NULL);

  // we build the left side of the assignment, which is an access operation
  AssignableElement *assgnbl = stmnt_assignment_get_assgnbl(stmnt);
  ObjectDerefList *obj_derefs = assgnbl->obj_derefs;
  AccessOperation *left_access = ObjectDerefList_To_AccessOperation(obj_derefs, var_table);

  // we now have to build the right side, which is a cfg expression, which basically is
  // a two operands expression. If the original expression is not a simple two operand
  // expression we have to create temporary variables to store intermidiate results
  Expression *value_expr = stmnt_assignment_get_value(stmnt);

  CFGStatementList_X_CFGExpression pair = Expression_To_CFGStatementList_X_CFGExpression(value_expr, var_table);

  CFGStatementList *cfg_statements = pair.cfg_statements;

  CFGExpression *cfg_value_expr = pair.cfg_expression;

  CFGStatement *cfg_assignment = CFGStatement_Create_Assignment(left_access, cfg_value_expr);

  CFGStatementList_Append(cfg_statements, cfg_assignment);

  return cfg_statements;
}

CFG *CFGBuilder_Build(ASTProgram *program) {
  assert(program != NULL);
  StructDeclarationList *structs = program->struct_declarations;
  FunctionDeclarationList *functions = program->func_declarations;
  Statement *global_statements = program->global_statement;

  FOR_EACH(StructDeclarationList, struct_it, structs) {
    strct_decl_print(struct_it->node, stdout);
    fprintf(stdout, "\n\n");
  }

  // assert(strct_decl_list_size(structs) == 0);
  assert(func_decl_list_size(functions) == 0);
  assert(stmnt_is_block(global_statements));

  StatementList *statements = stmnt_block_get_body(global_statements);
  GlobalVariablesTable *var_table = GlobalVariablesTable_Create();

  CFGStatementList *cfg_statements = CFGStatementList_CreateEmpty();

  FOR_EACH(StatementList, stmnt_it, statements) {
    
    Statement *stmnt = stmnt_it->node;
    assert(stmnt_is_assignment(stmnt) || stmnt_is_declaration(stmnt));

    if (stmnt_is_assignment(stmnt)) {
      CFGStatementList *tmp_cfg_stmnts = AssignmentStatement_To_CFGStatementList(stmnt, var_table);
      cfg_statements = CFGStatementList_Concat(cfg_statements, tmp_cfg_stmnts);
    } 
    else if (stmnt_is_declaration(stmnt_it->node)) {
      NameTypeBinding *nt_bind = stmnt_declaration_get_nt_bind(stmnt);
      Identifier *name = nt_bind->name;
      Type *type = nt_bind->type;

      assert(type_is_basic(type) || type_is_struct(type));

      Variable *decl_var = Variable_Create_Pointer(
        next_var_idx(),
        idf_copy_identifier(name),
        type_copy(type));
        
      GlobalVariablesTable_Add_Variable(var_table, decl_var);

      CFGStatement *cfg_stmnt = CFGStatement_Create_Declaration(*decl_var);
      CFGStatementList_Append(cfg_statements, cfg_stmnt);

      // if (stmnt_declaration_has_init_values(stmnt)) {
      //   ExpressionList *init_values = stmnt_declaration_get_init_values(stmnt);
      //   assert(expr_list_size(init_values) == 1);

      //   Expression *init_value = expr_list_get_at(init_values, 0);
      //   CFGExpression *cfg_init_value_expr = Expression_To_CFGExpression(init_value, var_table);
      //   assert(cfg_init_value_expr != NULL);

      //   AccessOperation *access_op = AccessOperation_Create_Variable_Access(*decl_var);
      //   CFGStatement *cfg_init_assgnmt = CFGStatement_Create_Assignment(access_op, cfg_init_value_expr);
      //   CFGStatementList_Append(cfg_statements, cfg_init_assgnmt);
      // }
    }

    stmnt_print(stmnt_it->node, stdout);
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "\n");
  CFGStatementList_Print(cfg_statements, stdout);

  CFGStatementList_Destroy(cfg_statements);

  fprintf(stdout, "\n");
  GlobalVariablesTable_Print(var_table, stdout);
  GlobalVariablesTable_Destroy(var_table);

  return NULL;
}