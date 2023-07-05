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

DONE
ProgramNodeData:
  ASTNode *func_declarations;
  ASTNode *global_statements;

DONE
ParseProgram():
  while (!END_TOKEN)
    if (FUNC_TOKEN)
      functions.append(ParseFunctionDeclaration())
    else 
      statements.append(ParseStatement())
  ret ProgramNode(functions, statements)


STRING AND ARRAY TYPES

  Now i want to add two new data types: array and string:
    - for string i have to be able to parse string literals (meaning somethign between "");
    - for arrays what i want to do is having instead of var keyword the arr keyword and to 
      declare an array of size 10 you have to write arr[10] my_array;

  Having that i would have new types of operands and params (STRING and ARRAY_DEREF), and i 
  would have to distinguish between variables declaration and array declaration. Also in an
  assignment the left hand side could be a variable or a array dereference. In the case of
  a variable is sufficient to store the identifier, in the case of array we will need to 
  store both the identifer and the integer index of the dereference. So now the assignment 
  store a left value (variable identifier or array dereference) and right value (meaning
  an expression).

  Notes on the implementation of string data type:

    I have added the possibility to have string literals as values of variables or operands. 
    The possibility to accept string as values in certain expression will resolve in a 
    later stage of compilation. There will be operands and operations on strings [1]. The matter
    of having the string param type is still not resolved. The idea is that we only need two
    type of parameters, the ones used in function declarations (just identifiers, for the moment),
    and the ones used in a function call, which are all expressions. So it is a consequence to
    think that neither the integer nor the string parameter type are necessary.  

    [1]: such as concat, len, prefix, etc ...

  Parsing array declaration:

    DONE
    ParseArrayDeclaration():
      ARR_TOKEN
      OPEN_SQUARE_TOKEN
      INTEGER_TOKEN::size
      CLOSE_SQUARE_TOKEN
      IDENTIFIER::name

    DONE
    ArrayDeclarationNodeData:
      Identifier name;
      Int size;
      ASTNodeList init_values;  // to upgarde to ExpressionList

    The next step is to add the possibility to initialize an array in this syntax:
      arr[5] bar = [1, 2, "hello", 3, "foo"];
    To do this i would use a list (or maybe an array because the size is known) of
    expressions that would initialize the values in sequence.

    DONE
    Now i have to accept array dereference as an operand in an expression. The operand
    has to store the identifier of the array and the index, this has to be an 
    expression (that later will be checked to be an integer expression).

DONE
Should i create an AssignableElement data structure to hold data of elements that
can be put on the left hand side of an assignment (so a variable or an array
dereference)? I think so. If I do so the assignment statement has to change the data.
The Identifier attribute need to go and in its place an AssignableElement will
be placed. For the moment just two types of AssignableElement will be able: 
  - VAR,
  - ARRAY_DEREF.

Pointers:

  Syntax: 
    ptr var k1;              declaration
    ptr var k2 = ptr(f);     declaration with initialization
    var foo = val(1k);       pointer dereference

Struct
  Syntax
    arr[20] f;
    data point { 
      var t, 
      var p, 
      arr[20] f 
    };
    var p = point{ 20, 4, f };

NOTE: i should give types to the parameters accepted by a function. For example:
  foo(var t, var p, arr[20] f) { ... }

I now want to change the type system in my language. I want types. I'll get rid of
var keyword and I will have 2 basics type: int, string, and 2 ways to declare
new types: data { ... } and <type> arr[size].
Steps:
  1) add to the lexer the appropriate keywords;
  2) modify the AST to have types in the declarations;
  3) modify the parser to generate the appropriate AST;

DONE
TODO: i have to change how funccall works, i want them to have a list of expression,
not a list of parameters. 


I want:
  variable declarations, function definition parameters and struct attributes to hold 
  an instance of an object of the form < name, type >, where name is the name of that 
  variable/attribute/parameter and the type is the type of that entity. Thid object 
  will be called NameTypeBinding { Identifier *name, Type *type }.

DONE
Now we have to parse types as a separate method:
  grammar for declaration: 
    DECLARATION: TYPE "::" IDENTIFIER ( '=' EXPRESSION ) ';'
  ex:
    int arr[5] :: int_array = [1, 2, 3, 4, 5];

  DONE
  # let say we know we are inside a declaration of some sort(var, param or attribute)
  ParseType(start_type):
    ParseArrayType()::array_type
    if (array_type)
      # add start_type to end of array_type chain
    else
      ret Type(INT_TYPE)

  DONE
  ParseDeclaration():
    ParseType()::type
    "::"
    IDENTIFIER::name
    if ('=')
      ParseExpression()::init_value
    ret DeclarationNode(NameTypeBinding(name, type), init_value)

DONE
I've implemented the parsing of types in variable declaration, now i have to do it
also for function declaration params. To do that before i have to add NameTypeBinding 
somewhere inside Paramater. 

NOTE: for the moment I do not implement multiple dimension arrays, not because it is
difficoult to parse the type, that's trivial, the hard part i think would be to create
and manage the data structure to hold the possible initial values. 

TODO: another thing we should do is to permit to have function parameters to be array
      of undeclared sizes. Such as: func foo(int arr[?], int size) { ... }

Struct:

  The moment is arrived to parse user defined data structure. This will change the things 
  a bit. For example we will need struct dereferencing. We have to able that given a struct:
    data my_data { int :: x, string :: s }
  and an object: 
    my_data :: o; 
  to dereference the single attributes of the o object in this manner:
    o.x = 10;
    o.s = "Hello, World";

============================================================================================

TODO list:
  [x] array initialization;
  [x] AssignableElement;
  [x] left unary operations (such as - and !);
  [ ] string operations;
  [ ] data struct;
  [ ] pointers;
  [x] change funccall to have list of exprs;
  [x] add types;
   |  [x] Type;
   |  [x] NameTypeBinding;
   |  [x] add NameTypeBinding to func declarations params, var declaration;
  [ ] undefined arr sizes in function declaration parameters

============================================================================================
*/