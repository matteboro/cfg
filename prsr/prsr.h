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
  if (lookhaed.type == token_type)
  {
    prsr_next_token();
    return;
  }
  PRSR_ERROR();
}

Expression *prsr_parse_funccall(Token token)
{
  char *func_name = lxr_get_token_data_as_cstring(token);
  fprintf(stdout, "\n%s\n", func_name);
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
    Token token = lookhaed;
    prsr_match(IDENTIFIER_TOKEN);
    if (lookhaed.type == OPEN_PAREN_TOKEN)
    {
      Expression *result = prsr_parse_funccall(token);
      return result;
    }
    return expr_create_operand_expression_from_token(token);
  }
  case INTEGER_TOKEN:
  {
    Token token = lookhaed;
    prsr_match(INTEGER_TOKEN);
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

ASTNode *prsr_parse_expression_node()
{
  Expression *expression = prsr_parse_expression();
  return ast_create_expression_node(expression);
}

ASTNode *prsr_parse_assignment()
{
  PRSR_DEBUG_PRINT();
  Token id_token = lookhaed;
  prsr_match(IDENTIFIER_TOKEN);
  prsr_match(EQUAL_TOKEN);
  ASTNode *expression_node = prsr_parse_expression_node();
  prsr_match(SEMICOLON_TOKEN);
  return ast_create_assignment_node(idf_create_identifier_from_token(id_token), expression_node);
}

ASTNode *prsr_parse(const char *data)
{

  Lexer lexer = lxr_init(data);
  lexer_ptr = &lexer;

  prsr_next_token();

  ASTNode *ast = prsr_parse_assignment();

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