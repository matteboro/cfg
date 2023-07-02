#include "prsr/prsr.h"

int main () {

  Identifier *id = idf_create_identifier("boo");
  idf_print_identifier(id, stdout); fprintf(stdout, "\n");
  idf_dealloc_identifier(id);


  id = idf_create_identifier("boo");
  Operand *op = oprnd_create_operand(IDENTIFIER_OPERAND, id);
  oprnd_print_operand(op, stdout); fprintf(stdout, "\n");
  oprnd_dealloc_operand(op);

  Expression *expr = prsr_parse_expression_from_string("(boo * f) + bar(69, foo, 420 - 69 * baz)");
  expr_print_expression(expr, stdout); fprintf(stdout, "\n");
  expr_dealloc_expression(expr);

  return 0;
}

/*

Function: {
  Name: foo,
  Params: [ bar, zed ],
  Code: [
    Assignment: {
      Variable: id.baz,
      Expression: (baz * 2) - 69
    },
    ...
    Declaration: {
      Variable: id.foo,
      Expression: 7 * id.bar
    }
  ]
}

DONE
now i want the declaration AST node:
DeclarationNodeData:
  Identifier id,
  bool has_init,
  ASTNode expression

DONE
StatementsNodeData
  ASTNodeList

DONE
FunctionDeclarationNodeData:
  char* name, ParamaterList params, ASTNode statements

DONE
ParseAssignemnt():
  IDENTIFIER::id
  '='
  ParseExpression()::expression

DONE
ParseVariableDeclaration():
  VAR_TOKEN
  IDENTIFIER::id
  if ('=')
    ParseExpression()::expression
  else
    NULL::expression

DONE
ParseStatement():
  if (VAR_TOKEN) 
    ParseVariableDeclaration()::statement
  else if ('{')
    ParseStatements()::statements
    ret statements
  else
    ParseAssignment()::statement
  ';'
  ret statement

DONE
ParseStatements(): (for the moment a list of variable declarations and assignments)
  '{'
  while (!'}')
    ParseStatement()::curr_statement
    statements.append(curr_statement)
  '}'

DONE
ParseFuncDeclParams():
  while (true)
    IDENTIFIER::param
    params.append(param)
    if (',')
      ','
    else
      break;
  ret params

DONE
ParseFunctionDeclaration():
  FUNC_TOKEN 
  IDENTIFIER::func_name 
  '(' 
  ParseFuncDeclParams()::params 
  ')' 
  ParseStatements()::body 

DONE
ConditionalStatementNodeData:
  Expression *guard,
  ASTNode *body

TODO: add else and elif parsing --> DONE
ParseIf(first):
  if (first)
    IF_TOKEN
  else
    ELIF_TOKEN
  ParseExpression()::expression
  ParseStatements()::if_body
  if (ELSE_TOKEN)
    ParseStatements()::else_body
    ret ElseIfNode(expression, if_body, else_body)
  else if (ELIF_TOKEN)
    ParseIf(False)::else_body
    ret ElseIfNode(expression, if_body, else_body)
  else
    ret IfNode(expression, if_body)

DONE
ParseWhile():
  WHILE_TOKEN
  ParseExpression()::expression
  ParseStatements()::body

DONE
IfElseNodeData:
  ASTNode *guard,
  ASTNode *if_body,
  ASTNode *else_body,

DONE
NOTE: in case of chains of elif in the else else_body attribute we 
      will put another IfElseNode with the guard of the elif. We 
      do that until we find an else, and that's where we stop.

Next step is to parse an entire program. In my language a program is made
of a list of function declaration(definition), that are visible across the 
all program, and a series of global statements. There is a special function 
called main() that is called at the end of the run of all the global 
statements. The global statements are called in the order they are written.
The variable declared in the global space are visible inside every function.
*/