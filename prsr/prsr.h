#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lxr/lxr.h"
#include "../expr/expr.h"
#include "ast.h"

/*

ASSGNM     :: [ IDENTIFIER_TOKEN - EQUAL_TOKEN - < EXPRESSION > - SEMICOLON_TOKEN ]

EXPRESSION :: [ < TERM > ( ( PLUS_TOKEN | MINUS_TOKEN ) - < TERM > )* ]

TERM       :: [ < FACTOR > ( ( ASTERISK_TOKEN | SLASH_TOKEN ) - < FACTOR > )* ]

FACTOR     :: [ IDENDIFIER_TOKEN ],
              [ INTEGER_TOKEN ],
              [ OPEN_PAREN_TOKEN - < EXPRESSION > - CLOSE_PAREN_TOKEN ]
*/

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
  fprintf(stdout, "error inside function: %s\n", __FUNCTION__); \
  exit(1);

ASTNode *prsr_parse_assignment();
Expression *prsr_parse_expression(), *prsr_parse_term(), *prsr_parse_factor();

static Token lookhaed;
Lexer *lexer_ptr;

void prsr_match(TokenType token_type)
{
  PRSR_MATCH_DEBUG_PRINT();
  if (lookhaed.type == token_type) {
    prsr_next_token();
    return;
  }
  fprintf(stdout, "expected: ");
  lxr_print_token_type(token_type); 
  fprintf(stdout, "\ngot: ");
  lxr_print_token_type(lookhaed.type); 
  fprintf(stdout, " at position: %d\n", lookhaed.position);
  PRSR_ERROR();
}

Expression *prsr_parse_funccall(Token token)
{
  char *func_name = lxr_get_token_data_as_cstring(token);
  // fprintf(stdout, "\n%s\n", func_name);
  prsr_match(OPEN_PAREN_TOKEN);
  ParameterList *params = prmt_list_create_empty();
  if (lookhaed.type != CLOSE_PAREN_TOKEN)
  {
    while (True)
    {
      Expression *tmp_expr = prsr_parse_expression();
      Parameter *tmp_param = prmt_create_expression_param(tmp_expr);
      prmt_list_append(params, tmp_param);
      if (lookhaed.type != COMMA_TOKEN)
        break;
      prsr_match(COMMA_TOKEN);
    }
  }
  FunctionCall *func_call = funccall_create(func_name, params);
  Expression *result = expr_create_funccall_operand_expression(func_call);
  free(func_name);
  prsr_match(CLOSE_PAREN_TOKEN);
  return result;
}

Expression *prsr_parse_factor()
{
  PRSR_DEBUG_PRINT();
  switch (lookhaed.type)
  {
  case IDENTIFIER_TOKEN:
  {
    Token id_token = lookhaed;
    prsr_match(IDENTIFIER_TOKEN);
    if (lookhaed.type == OPEN_PAREN_TOKEN) 
    {
      Expression *result = prsr_parse_funccall(id_token);
      return result;
    } 
    else if (lookhaed.type == OPEN_SQUARE_TOKEN) 
    {
      prsr_match(OPEN_SQUARE_TOKEN);
      Expression *index_expr = prsr_parse_expression();
      prsr_match(CLOSE_SQUARE_TOKEN);
      return expr_create_array_deref_operand_expression(
        idf_create_identifier_from_token(id_token),
        index_expr);
    }
    return expr_create_operand_expression_from_token(id_token);
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
    Expression *expression = prsr_parse_expression();
    prsr_match(CLOSE_PAREN_TOKEN);
    return expression;
  }
  default:
    PRSR_ERROR();
  }
}

Expression *prsr_parse_term()
{
  PRSR_DEBUG_PRINT();
  Expression *root = prsr_parse_factor();
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

ASTNode *prsr_parse_expression_node()
{
  Expression *expression = prsr_parse_equation();
  return ast_create_expression_node(expression);
}

ASTNode *prsr_parse_var_declaration() 
{
  PRSR_DEBUG_PRINT();
  prsr_match(VAR_TOKEN);
  Token id_token = lookhaed;
  prsr_match(IDENTIFIER_TOKEN);
  ASTNode *expression_node = NULL;
  if (lookhaed.type == EQUAL_TOKEN){
    prsr_match(EQUAL_TOKEN);
    expression_node = prsr_parse_expression_node();
  }
  return ast_create_declaration_node(idf_create_identifier_from_token(id_token), expression_node);
}

ASTNode *prsr_parse_arr_declaration() 
{
  PRSR_DEBUG_PRINT();
  prsr_match(ARR_TOKEN);
  prsr_match(OPEN_SQUARE_TOKEN);
  Token size_token = lookhaed;
  prsr_match(INTEGER_TOKEN);
  prsr_match(CLOSE_SQUARE_TOKEN);
  Token id_token = lookhaed;
  prsr_match(IDENTIFIER_TOKEN);
  return ast_create_array_declaration_node(
    idf_create_identifier_from_token(id_token),
    lxr_get_integer_value_of_integer_token(size_token),
    NULL  // for the moment we do not support init values for arrays declaration
  );
}

ASTNode *prsr_parse_assignment()
{
  PRSR_DEBUG_PRINT();
  Token id_token = lookhaed;
  prsr_match(IDENTIFIER_TOKEN);
  if (lookhaed.type == OPEN_SQUARE_TOKEN) {
    prsr_match(OPEN_SQUARE_TOKEN);
    Expression *index = prsr_parse_expression();
    prsr_match(CLOSE_SQUARE_TOKEN);
    prsr_match(EQUAL_TOKEN);
    ASTNode *expression_node = prsr_parse_expression_node();
    return ast_create_assignment_node(
      assgnbl_create_arr_deref_assignable(
        idf_create_identifier_from_token(id_token),
        index), 
      expression_node);
  } else {
    prsr_match(EQUAL_TOKEN);
    ASTNode *expression_node = prsr_parse_expression_node();
    return ast_create_assignment_node(
      assgnbl_create_var_assignable(idf_create_identifier_from_token(id_token)), 
      expression_node);
  }
}

ASTNode *prsr_parse_while_statement(); 
ASTNode *prsr_parse_if_statement(bool first);
ASTNode *prsr_parse_statements();

ASTNode *prsr_parse_statement() 
{
  PRSR_DEBUG_PRINT();
  ASTNode *statement;
  if (lookhaed.type == VAR_TOKEN) {
    statement = prsr_parse_var_declaration();
  } else if (lookhaed.type == OPEN_CURLY_TOKEN){
    return prsr_parse_statements();
  } else if (lookhaed.type == IF_TOKEN){
    return prsr_parse_if_statement(True);
  } else if (lookhaed.type == WHILE_TOKEN){
    return prsr_parse_while_statement();
  } else if (lookhaed.type == ARR_TOKEN){
    statement = prsr_parse_arr_declaration();
  } else {
    statement = prsr_parse_assignment();
  }
  prsr_match(SEMICOLON_TOKEN);
  return statement;
}

ASTNode *prsr_parse_statements() 
{
  PRSR_DEBUG_PRINT();
  ASTNodeList *node_list = ast_list_create_empty();
  prsr_match(OPEN_CURLY_TOKEN);
  while (lookhaed.type != CLOSE_CURLY_TOKEN) {
    ASTNode *statement = prsr_parse_statement();
    ast_list_append(node_list, statement);
  }
  prsr_match(CLOSE_CURLY_TOKEN);
  return ast_create_node_list_node(node_list);
}

ParameterList *prsr_parse_func_declaration_params() 
{
  ParameterList *params = prmt_list_create_empty();
  while (True) {
    if (lookhaed.type != IDENTIFIER_TOKEN)
      break;
    Token id = lookhaed;
    prsr_match(IDENTIFIER_TOKEN);

    prmt_list_append(params, prmt_create_identifer_param(idf_create_identifier_from_token(id)));

    if (lookhaed.type != COMMA_TOKEN)
      break;
    prsr_match(COMMA_TOKEN);
  }
  return params;
}

ASTNode *prsr_parse_func_declaration() 
{
  PRSR_DEBUG_PRINT();
  prsr_match(FUNC_TOKEN);
  Token func_name_id = lookhaed;
  prsr_match(IDENTIFIER_TOKEN);
  prsr_match(OPEN_PAREN_TOKEN);
  ParameterList *params = prsr_parse_func_declaration_params();
  prsr_match(CLOSE_PAREN_TOKEN);
  ASTNode *body = prsr_parse_statements();
  return ast_create_func_declaration_node(
    idf_create_identifier_from_token(func_name_id),
    params, 
    body);
}

// TODO: we should also parse the possible else and the chain of elif
ASTNode *prsr_parse_if_statement(bool first) 
{
  if (first)
    prsr_match(IF_TOKEN);
  else  
    prsr_match(ELIF_TOKEN);
  ASTNode *expression = prsr_parse_expression_node();
  ASTNode *if_body = prsr_parse_statements();
  if (lookhaed.type == ELSE_TOKEN) {
    prsr_match(ELSE_TOKEN);
    ASTNode *else_body = prsr_parse_statements();
    return ast_create_if_else_node(expression, if_body, else_body);
  } else if (lookhaed.type == ELIF_TOKEN) {
    ASTNode *else_body = prsr_parse_if_statement(False);
    return ast_create_if_else_node(expression, if_body, else_body);
  } else {
    return ast_create_if_node(expression, if_body);
  }
}

ASTNode *prsr_parse_while_statement() 
{
  prsr_match(WHILE_TOKEN);
  ASTNode *expression = prsr_parse_expression_node();
  ASTNode *body = prsr_parse_statements();
  return ast_create_while_node(expression, body);
}

ASTNode *prsr_parse_program()
{
  ASTNodeList *functions = ast_list_create_empty();
  ASTNodeList *global_stmnts = ast_list_create_empty();

  while (lookhaed.type != END_TOKEN) {
    if (lookhaed.type == FUNC_TOKEN)
      ast_list_append(functions, prsr_parse_func_declaration());
    else
      ast_list_append(global_stmnts, prsr_parse_statement());
  }
  return ast_create_program_node(
    ast_create_node_list_node(functions), 
    ast_create_node_list_node(global_stmnts));
}

ASTNode *prsr_parse(const char *data)
{
  Lexer lexer = lxr_init(data);
  lexer_ptr = &lexer;

  prsr_next_token();

  ASTNode *ast = prsr_parse_program();

  if (lookhaed.type == END_TOKEN)
    return ast;

  return NULL;
}

Expression *prsr_parse_expression_from_string(const char *data)
{
  Lexer lexer = lxr_init(data);
  lexer_ptr = &lexer;

  prsr_next_token();

  Expression *expression = prsr_parse_expression();

  if (lookhaed.type == END_TOKEN)
    return expression;

  expr_dealloc_expression(expression);
  return NULL;
}