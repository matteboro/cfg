#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lxr/lxr.h"
#include "../expr/expr.h"
#include "prgrm.h"

// #define PRSR_DEBUG
#ifdef PRSR_DEBUG
#define PRSR_DEBUG_PRINT() fprintf(stdout, "%s\n", __FUNCTION__);
#define PRSR_MATCH_DEBUG_PRINT()        \
  fprintf(stdout, "%s(", __FUNCTION__); \
  lxr_print_token_type(token_type);     \
  fprintf(stdout, ")\n");
#define prsr_next_token()               \
  fprintf(stdout, "  ");                \
  lookhaed = lxr_next_token(lexer_ptr); \
  lxr_print_token(lookhaed);            \
  fprintf(stdout, "\n");
#else
#define PRSR_DEBUG_PRINT()
#define PRSR_MATCH_DEBUG_PRINT()
#define prsr_next_token() lookhaed = lxr_next_token(lexer_ptr);
#endif
#define PRSR_ERROR()                                            \
  fprintf(stdout, "ERROR inside function: %s\n\n", __FUNCTION__); \
  fprintf(stdout, "token: "); \
  lxr_print_token_type(lookhaed.type); \
  fprintf(stdout, " at position: %d\n", lookhaed.position); \
  exit(1);

#define PRSR_CALL_STACK() caller_stack_add(__FUNCTION__)

Statement *prsr_parse_assignment(Token start_deref);
Expression *prsr_parse_expression(); 
Expression *prsr_parse_term(); 
Expression *prsr_parse_factor(); 
Expression *prsr_parse_equation(); 
Expression *prsr_parse_str_expression();
Type *prsr_parse_non_array_type();

static Token lookhaed;
static Token prev_lookhaed;
Lexer *lexer_ptr;
#define CALLER_STACK_SIZE 10

static char *caller_stack[CALLER_STACK_SIZE]; 
static size_t caller_stack_pos = 0;

void caller_stack_add(const char *func_name) {

  char *func_name_copy = (char *) malloc(sizeof(char)*strlen(func_name)+1);
  strcpy(func_name_copy, func_name);

  if (caller_stack_pos >= CALLER_STACK_SIZE) 
    free(caller_stack[CALLER_STACK_SIZE-1]);

  for (size_t i=CALLER_STACK_SIZE-1; i > 0; --i) {
    caller_stack[i] = caller_stack[i-1];
  }

  caller_stack[0] = func_name_copy;

  if (caller_stack_pos < CALLER_STACK_SIZE) 
    ++caller_stack_pos;
}

void caller_stack_print(FILE *file) {
  fprintf(file, "  prsr_match calling stack:\n");
  for (size_t i = 0; i < caller_stack_pos; ++i) {
    fprintf(file, "    - %lu - %s\n", i, caller_stack[i]);
  }
}

void caller_stack_dealloc() {
  for (size_t i=0; i<caller_stack_pos; ++i)
    free(caller_stack[i]);
}

Token __prsr_match(TokenType token_type, const char *caller)
{
  PRSR_MATCH_DEBUG_PRINT();
  if (lookhaed.type == token_type) {
    Token result = lookhaed;
    prev_lookhaed = lookhaed;
    prsr_next_token();
    return result;
  }
  fprintf(stdout, "ERROR inside function: %s called from function: %s\n\n", __FUNCTION__, caller);
  caller_stack_print(stdout);
  fprintf(stdout, "\n");
  fprintf(stdout, "  just parsed: ");
  lxr_print_token_type(prev_lookhaed.type); 
  fprintf(stdout, "\n  expected: ");
  lxr_print_token_type(token_type); 
  fprintf(stdout, "\n  got: ");
  lxr_print_token_type(lookhaed.type); 
  fprintf(stdout, " at position: %d\n\n", lookhaed.position);
  // FileInfo file_info = lxr_get_current_file_info(lexer_ptr);
  single_line_file_info_print_context(lookhaed.file_info, stdout); fprintf(stdout, "\n\n");
  exit(1);
  return lxr_create_null_token();
}

#define prsr_match(token_type)  __prsr_match(token_type, __FUNCTION__)
#define prsr_match_any()  __prsr_match(lookhaed.type, __FUNCTION__)
#define prsr_next_is(token_type) lookhaed.type == token_type 
//// PARSE EXPRESSION

FunctionCall *prsr_parse_funccall(Token func_name)
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  prsr_match(OPEN_PAREN_TOKEN);
  ExpressionList *params_values = expr_list_create_empty();
  if (lookhaed.type != CLOSE_PAREN_TOKEN)
  {
    while (True)
    {
      Expression *tmp_expr = prsr_parse_equation();
      expr_list_append(params_values, tmp_expr);
      if (lookhaed.type != COMMA_TOKEN)
        break;
      prsr_match(COMMA_TOKEN);
    }
  }
  FileInfo file_info = file_info_merge(func_name.file_info, lookhaed.file_info);
  FunctionCall *func_call = funccall_create(idf_create_identifier_from_token(func_name), params_values, file_info);
  prsr_match(CLOSE_PAREN_TOKEN);
  return func_call;
}

ObjectDerefList *prsr_parse_deref_list(Token start_deref) {
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  bool first = True;
  ObjectDerefList *obj_derefs = obj_drf_list_create_empty();
  while (True) {
    Token first_token;
    if (first) {
      first = False;
      first_token = start_deref;
    } 
    else {
      first_token = prsr_match(lookhaed.type);
    } 

    ObjectDeref *new_obj_deref = NULL;
    // parse normal deref, array deref
    if (first_token.type == IDENTIFIER_TOKEN) {
      // parse array deref
      if (lookhaed.type == OPEN_SQUARE_TOKEN) {
        FileInfo file_info = first_token.file_info;
        prsr_match(OPEN_SQUARE_TOKEN);
        Expression *index_expr = prsr_parse_equation();
        Token close_square_token = prsr_match(CLOSE_SQUARE_TOKEN);

        new_obj_deref = 
          obj_drf_create_array_type_deref(
            idf_create_identifier_from_token(first_token), 
            index_expr, 
            file_info_merge(file_info, close_square_token.file_info));
      } 
      // parse normal deref
      else {
        new_obj_deref = obj_drf_create_struct_or_basic_type_deref(idf_create_identifier_from_token(first_token));
      }
    } 
    // parse single element pointer deref
    else if (first_token.type == OPEN_SQUARE_TOKEN) {
      Token id_token = prsr_match(IDENTIFIER_TOKEN);
      Token close_square_token = prsr_match(CLOSE_SQUARE_TOKEN);

      new_obj_deref = 
        obj_drf_create_single_element_pointer_deref(
          idf_create_identifier_from_token(id_token),
          file_info_merge(first_token.file_info, close_square_token.file_info));
    } 
    else {
      PRSR_ERROR();
    }

    obj_drf_list_append(obj_derefs, new_obj_deref);

    if (lookhaed.type == POINT_TOKEN)
      prsr_match(POINT_TOKEN);
    else
      break;
  }
  return obj_derefs;
}

Expression *prsr_parse_factor()
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  switch (lookhaed.type)
  {
  case IDENTIFIER_TOKEN:
  {
    Token start_deref = prsr_match(IDENTIFIER_TOKEN);
    if (lookhaed.type == OPEN_PAREN_TOKEN) 
    {
      Expression *result = expr_create_funccall_operand_expression(prsr_parse_funccall(start_deref));
      return result;
    } 
    ObjectDerefList *derefs = prsr_parse_deref_list(start_deref);
    return expr_create_object_deref_operand_expression(derefs);
  }
  case INTEGER_TOKEN:
  {
    Token token = lookhaed;
    prsr_match(INTEGER_TOKEN);
    return expr_create_operand_expression_from_token(token);
  }
  case STRING_TOKEN:
  {
    Token token = lookhaed;
    prsr_match(STRING_TOKEN);
    return expr_create_operand_expression_from_token(token);
  }
  case OPEN_PAREN_TOKEN:
  {
    prsr_match(OPEN_PAREN_TOKEN);
    Expression *expression = prsr_parse_equation();
    prsr_match(CLOSE_PAREN_TOKEN);
    return expression;
  }
  case MINUS_TOKEN:
  {
    Token op = prsr_match(MINUS_TOKEN);
    Expression *expression = prsr_parse_factor();
    return expr_create_unary_expression(expression, MINUS_UNARY_OPERATION, file_info_merge(op.file_info, expression->file_info));
  }
  case EXCL_POINT_TOKEN:
  {
    Token op = prsr_match(MINUS_TOKEN);
    prsr_match(EXCL_POINT_TOKEN);
    Expression *expression = prsr_parse_factor();
    return expr_create_unary_expression(expression, NOT_UNARY_OPERATION, file_info_merge(op.file_info, expression->file_info));
  }
  case LESS_TOKEN:
  {
    return prsr_parse_str_expression();
  }
  default:
    PRSR_ERROR();
  }
}

Expression *prsr_parse_str_expression() {
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  Expression *root = NULL;
  if (lookhaed.type == LESS_TOKEN) {
    Token left_bound = prsr_match(LESS_TOKEN);
    root = prsr_parse_expression();
    Token right_bound = prsr_match(GREATER_TOKEN);
    root = expr_create_unary_expression(root, STR_LEN_UNARY_OPERATION, file_info_merge(left_bound.file_info, right_bound.file_info));
  } else {
    root = prsr_parse_factor();
    while (lookhaed.type == BAR_TOKEN)
    {
      prsr_match(lookhaed.type);
      Expression *right = prsr_parse_factor();
      root = expr_create_binary_expression(root, STR_CONCAT_OPERATION, right);
    } 
  }
  return root;
}

Expression *prsr_parse_term()
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  Expression *root = prsr_parse_str_expression();
  while (lookhaed.type == ASTERISK_TOKEN || lookhaed.type == SLASH_TOKEN)
  {
    OperationType op = lookhaed.type == ASTERISK_TOKEN ? MULT_OPERATION : DIV_OPERATION;
    prsr_match(lookhaed.type);
    Expression *right = prsr_parse_factor();
    root = expr_create_binary_expression(root, op, right);
  }
  return root;
}

Expression *prsr_parse_expression()
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  Expression *root = prsr_parse_term();
  while (lookhaed.type == PLUS_TOKEN || lookhaed.type == MINUS_TOKEN)
  {
    OperationType op = lookhaed.type == PLUS_TOKEN ? SUM_OPERATION : SUB_OPERATION;
    prsr_match(lookhaed.type);
    Expression *right = prsr_parse_term();
    root = expr_create_binary_expression(root, op, right);
  }
  return root;
}

OperationType prsr_token_type_to_operation_type(TokenType type) 
{
  if (type == EQUAL_EQUAL_TOKEN)
    return EQ_OPERATION;
  if (type == NOT_EQUAL_TOKEN)
    return NEQ_OPERATION;
  if (type == GREATER_EQUAL_TOKEN)
    return GEQ_OPERATION;
  if (type == LESS_EQUAL_TOKEN)
    return LEQ_OPERATION;
  if (type == GREATER_TOKEN)
    return GE_OPERATION;
  if (type == LESS_TOKEN)
    return LE_OPERATION;
  PRSR_ERROR();
}

Expression *prsr_parse_equation()
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  Expression *left = prsr_parse_expression();
  Token op = lookhaed;
  if (lookhaed.type == EQUAL_EQUAL_TOKEN || lookhaed.type == GREATER_EQUAL_TOKEN ||
      lookhaed.type == LESS_EQUAL_TOKEN || lookhaed.type == NOT_EQUAL_TOKEN ||
      lookhaed.type == GREATER_TOKEN || lookhaed.type == LESS_TOKEN) {
    prsr_match(op.type);
    Expression *right = prsr_parse_expression();
    return expr_create_binary_expression(left, prsr_token_type_to_operation_type(op.type), right);
  } else {
    return left;
  }
}

Expression *prsr_parse_create_expression() {
  PRSR_CALL_STACK();
  Token create_token = prsr_match(CREATE_TOKEN);
  Type *type = prsr_parse_non_array_type();
  if (lookhaed.type != OPEN_SQUARE_TOKEN) {
    return 
      expr_create_create_expression(type, NULL, file_info_merge(create_token.file_info, type->file_info));
  }
  prsr_match(OPEN_SQUARE_TOKEN);
  Expression *size_expression = prsr_parse_equation();
  Token close_square_token = prsr_match(CLOSE_SQUARE_TOKEN);
  return 
    expr_create_create_expression(
      type, 
      size_expression, 
      file_info_merge(create_token.file_info, close_square_token.file_info));
}

Expression *prsr_parse_general_expression() {
  PRSR_CALL_STACK();
  if (lookhaed.type == CREATE_TOKEN)
    return prsr_parse_create_expression();
  return prsr_parse_equation();
}

//// END PARSE EXPRESSION

ExpressionList *prsr_parse_arr_initializations_values() {
  PRSR_CALL_STACK();
  ExpressionList *init_values = expr_list_create_empty();
  if (lookhaed.type != CLOSE_SQUARE_TOKEN)
  {
    while(True) 
    {
      Expression *expr = prsr_parse_general_expression();
      expr_list_append(init_values, expr);
      if (lookhaed.type != COMMA_TOKEN)
        break;
      prsr_match(COMMA_TOKEN);
    }
  }
  return init_values;
}

Type *prsr_parse_type(Token start_type) {
  PRSR_CALL_STACK();
  Type *type = NULL;
  if (start_type.type == INT_TYPE_TOKEN)
    type = type_create_int_type(start_type.file_info);
  else if (start_type.type == STRING_TYPE_TOKEN)
    type = type_create_string_type(start_type.file_info);
  else
    type = type_create_struct_type(idf_create_identifier_from_token(start_type));

  // NOTE: for the moment we do not support multi dimensional array
  // NOTE: for the moment we do no support array of pointers
  if (prsr_next_is(ARR_TOKEN)) {
    prsr_match(ARR_TOKEN);
    prsr_match(OPEN_SQUARE_TOKEN);
    int size = lxr_get_integer_value_of_integer_token(lookhaed);
    prsr_match(INTEGER_TOKEN);
    Token close_square_token = prsr_match(CLOSE_SQUARE_TOKEN);
    type = type_create_array_type(
      size, 
      type, 
      file_info_merge(type->file_info, close_square_token.file_info));
  } 
  else if (prsr_next_is(PTR_TOKEN) || prsr_next_is(STRONG_TOKEN)) {
    bool is_strong = False;
    if (prsr_next_is(STRONG_TOKEN)) {
      prsr_match(STRONG_TOKEN);
      is_strong = True;
    }
    Token ptr_token = prsr_match(PTR_TOKEN);
    type = type_create_pointer_type(type, is_strong, file_info_merge(type->file_info, ptr_token.file_info));
  }
  return type;
}

Type *prsr_parse_non_array_type() {
  PRSR_CALL_STACK();

  Token start_type = prsr_match(lookhaed.type);
  Type *type = NULL;

  if (start_type.type == INT_TYPE_TOKEN)
    type = type_create_int_type(start_type.file_info);
  else if (start_type.type == STRING_TYPE_TOKEN)
    type = type_create_string_type(start_type.file_info);
  else if (start_type.type == IDENTIFIER_TOKEN)
    type = type_create_struct_type(idf_create_identifier_from_token(start_type));
  // NOTE: we do not support ptr to ptr for the moment

  return type;
}

//// PARSE STATEMENT

Statement *prsr_parse_assignment(Token start_deref)
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  ObjectDerefList *derefs = prsr_parse_deref_list(start_deref);
  prsr_match(EQUAL_TOKEN);
  Expression *expression = prsr_parse_general_expression();
  AssignableElement *assgnbl = assgnbl_create(derefs);
  return stmnt_create_assignment(
    assgnbl, 
    expression, 
    file_info_merge(assgnbl->file_info, lookhaed.file_info));
}

Statement *prsr_parse_declaration(Token start_token) {
  PRSR_CALL_STACK();

  bool is_global = False;
  Token first_token = start_token;

  if (start_token.type == GLOBAL_TOKEN) {
    is_global = True;
    start_token = prsr_match_any();
  }

  Type *type = prsr_parse_type(start_token);

  prsr_match(DOUBLE_COLON_TOKEN);
  Token name_token = lookhaed;
  prsr_match(IDENTIFIER_TOKEN);

  ExpressionList *init_expr = NULL;
  if (lookhaed.type == EQUAL_TOKEN) {
    prsr_match(EQUAL_TOKEN);
    if (lookhaed.type == OPEN_SQUARE_TOKEN) {
      prsr_match(OPEN_SQUARE_TOKEN);
      init_expr = prsr_parse_arr_initializations_values();
      prsr_match(CLOSE_SQUARE_TOKEN);
    } 
    else {
      init_expr = expr_list_create_empty();
      expr_list_append(init_expr, prsr_parse_general_expression());
    }
    // TODO: add struct initialization using { val1, val2, ... val2 }
  }

  NameTypeBinding *nt_bind = nt_bind_create(idf_create_identifier_from_token(name_token), type);
  FileInfo file_info = file_info_merge(first_token.file_info, lookhaed.file_info);
  return stmnt_create_declaration(nt_bind, init_expr, is_global, file_info);
}

Statement *prsr_dispatch_id_started_statement() {
  PRSR_CALL_STACK();
  Statement *statement = NULL;

  Token start_token = prsr_match(lookhaed.type);

  if (start_token.type == OPEN_SQUARE_TOKEN) {
    statement = prsr_parse_assignment(start_token);
  }
  else if (lookhaed.type == EQUAL_TOKEN || 
          lookhaed.type == OPEN_SQUARE_TOKEN ||
          lookhaed.type == POINT_TOKEN) {
    statement = prsr_parse_assignment(start_token);
  } 
  else if (lookhaed.type == OPEN_PAREN_TOKEN) {
    statement = stmnt_create_funccall(
      prsr_parse_funccall(start_token), 
      file_info_merge(start_token.file_info, lookhaed.file_info));
  } 
  else {
    statement = prsr_parse_declaration(start_token);
  }
  return statement;
}

Statement *prsr_parse_while_statement(); 
Statement *prsr_parse_return_statement(); 
Statement *prsr_parse_if_statement(bool first);
Statement *prsr_parse_statements();

Statement *prsr_parse_statement() 
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  if (lookhaed.type == OPEN_CURLY_TOKEN) {
    return prsr_parse_statements();
  } else if (lookhaed.type == IF_TOKEN) {
    return prsr_parse_if_statement(True);
  } else if (lookhaed.type == WHILE_TOKEN) {
    return prsr_parse_while_statement();
  } else if (lookhaed.type == RETURN_TOKEN) {
    return prsr_parse_return_statement();
  }
  Statement *statement = NULL;
  statement = prsr_dispatch_id_started_statement();
  prsr_match(SEMICOLON_TOKEN);
  return statement;
}

Statement *prsr_parse_statements() 
{
  PRSR_DEBUG_PRINT();
  PRSR_CALL_STACK();
  StatementList *stmnt_list = stmnt_list_create_empty();
  Token open_curly_token = prsr_match(OPEN_CURLY_TOKEN);
  while (lookhaed.type != CLOSE_CURLY_TOKEN) {
    Statement *statement = prsr_parse_statement();
    stmnt_list_append(stmnt_list, statement);
  }
  Token close_curly_token = prsr_match(CLOSE_CURLY_TOKEN);
  return stmnt_create_block(stmnt_list, file_info_merge(open_curly_token.file_info, close_curly_token.file_info));
}

Statement *prsr_parse_return_statement() 
{
  PRSR_CALL_STACK();
  Token return_token = prsr_match(RETURN_TOKEN);
  Expression *ret_value = prsr_parse_expression();
  Token semicolon_token = prsr_match(SEMICOLON_TOKEN);
  return stmnt_create_return(ret_value, file_info_merge(return_token.file_info, semicolon_token.file_info));
}

Statement *prsr_parse_if_statement(bool first) 
{
  PRSR_CALL_STACK();
  Token keyword_token;
  if (first)
    keyword_token = prsr_match(IF_TOKEN);
  else  
    keyword_token = prsr_match(ELIF_TOKEN);
    
  Expression *condition = prsr_parse_expression();
  Statement *if_body = prsr_parse_statements();

  if (lookhaed.type == ELSE_TOKEN) {
    prsr_match(ELSE_TOKEN);
    Statement *else_body = prsr_parse_statements();
    return stmnt_create_if_else(
      condition, 
      if_body, 
      else_body, 
      file_info_merge(keyword_token.file_info, else_body->file_info));
  } 
  else if (lookhaed.type == ELIF_TOKEN) {
    Statement *else_body = prsr_parse_if_statement(False);
    return stmnt_create_if_else(
      condition, 
      if_body, 
      else_body, 
      file_info_merge(keyword_token.file_info, else_body->file_info));
  } 
  else {
    return stmnt_create_if_else(
      condition, 
      if_body, 
      NULL, 
      file_info_merge(keyword_token.file_info, if_body->file_info));
  }
}

Statement *prsr_parse_while_statement() 
{
  PRSR_CALL_STACK();
  Token while_token = prsr_match(WHILE_TOKEN);
  Expression *condition = prsr_parse_expression();
  Statement *body = prsr_parse_statements();
  return stmnt_create_while(condition, body, file_info_merge(while_token.file_info, body->file_info));
}

//// END PARSE STATEMENT

ParameterList *prsr_parse_func_declaration_params() 
{
  PRSR_CALL_STACK();
  ParameterList *params = prmt_list_create_empty();
  while (True) {
    // TODO: factorize, this is kind of the same code as in declaration
    Token start_type = lookhaed;
    prsr_match(lookhaed.type);
    Type *type = prsr_parse_type(start_type);
    prsr_match(DOUBLE_COLON_TOKEN);
    Token name_token = lookhaed;
    prsr_match(IDENTIFIER_TOKEN);
    prmt_list_append(
      params,
      prmt_create(
        nt_bind_create(
          idf_create_identifier_from_token(name_token),
          type
        )
      )
    );
    if (lookhaed.type != COMMA_TOKEN)
      break;
    prsr_match(COMMA_TOKEN);
  }
  return params;
}

FunctionDeclaration *prsr_parse_func_declaration() 
{
  PRSR_CALL_STACK();
  PRSR_DEBUG_PRINT();
  Token func_token = prsr_match(FUNC_TOKEN);
  Type *return_type = prsr_parse_type(prsr_match(lookhaed.type));
  prsr_match(DOUBLE_COLON_TOKEN);
  Token func_name_id = prsr_match(IDENTIFIER_TOKEN);
  prsr_match(OPEN_PAREN_TOKEN);
  ParameterList *params = NULL;
  if (lookhaed.type != CLOSE_PAREN_TOKEN)
    params = prsr_parse_func_declaration_params();
  else
    params = prmt_list_create_empty();
  prsr_match(CLOSE_PAREN_TOKEN);
  Statement *body = prsr_parse_statements();
  FunctionDeclaration *func_decl = func_decl_create(
    idf_create_identifier_from_token(func_name_id),
    return_type,
    params, 
    body, 
    file_info_merge(func_token.file_info, body->file_info));
  return func_decl;
}

StructDeclaration *prsr_parse_struct_declaration() {
  PRSR_CALL_STACK();
  Token data_token = prsr_match(DATA_TOKEN);
  Token name = prsr_match(IDENTIFIER_TOKEN);
  AttributeList *attributes = attrb_list_create_empty();  
  prsr_match(OPEN_CURLY_TOKEN);
  while (True) {
    Type *type = prsr_parse_type(prsr_match(lookhaed.type));
    prsr_match(DOUBLE_COLON_TOKEN);
    Token attr_name = prsr_match(IDENTIFIER_TOKEN);
    attrb_list_append(
      attributes, 
      attrb_create(
        nt_bind_create(
          idf_create_identifier_from_token(attr_name),
          type)));
    if (lookhaed.type == COMMA_TOKEN)
      prsr_match(COMMA_TOKEN);
    else
      break;
  }
  Token close_curly_token = prsr_match(CLOSE_CURLY_TOKEN);

  return strct_decl_create(
    idf_create_identifier_from_token(name),
    attributes,
    file_info_merge(data_token.file_info, close_curly_token.file_info)
  );
}

ASTProgram *prsr_parse_program()
{
  PRSR_CALL_STACK();
  FunctionDeclarationList *functions = func_decl_list_create_empty();
  StructDeclarationList *struct_declarations = strct_decl_list_create_empty();
  StatementList *global_stmnts = stmnt_list_create_empty();
  while (lookhaed.type != END_TOKEN) {
    if (lookhaed.type == FUNC_TOKEN)
      func_decl_list_append(functions, prsr_parse_func_declaration());
    else if (lookhaed.type == DATA_TOKEN)
      strct_decl_list_append(struct_declarations, prsr_parse_struct_declaration());
    else
      stmnt_list_append(global_stmnts, prsr_parse_statement());
  }
  return prgrm_create(struct_declarations, functions, stmnt_create_block(global_stmnts, file_info_create_null()));
}

ASTProgram *prsr_parse(File *file)
{
  Lexer lexer = lxr_init(file);
  lexer_ptr = &lexer;

  prsr_next_token();

  ASTProgram *ast = prsr_parse_program();

  caller_stack_dealloc();

  if (lookhaed.type == END_TOKEN)
    return ast;

  return NULL;
}

Expression *prsr_parse_expression_from_file(File *file)
{
  Lexer lexer = lxr_init(file);
  lexer_ptr = &lexer;

  prsr_next_token();

  Expression *expression = prsr_parse_equation();

  if (lookhaed.type == END_TOKEN)
    return expression;

  expr_dealloc_expression(expression);
  return NULL;
}

StructDeclaration *prsr_parse_struct_declaration_from_file(File *file) {
  Lexer lexer = lxr_init(file);
  lexer_ptr = &lexer;

  prsr_next_token();

  StructDeclaration *struct_decl = prsr_parse_struct_declaration();

  if (lookhaed.type == END_TOKEN)
    return struct_decl;

  strct_decl_dealloc(struct_decl);
  return NULL;
}