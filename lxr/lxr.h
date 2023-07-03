#ifndef LXR_HEADER
#define LXR_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TODO() fprintf(stdout, "TODO: %s is not implemented\n", __FUNCTION__); exit(1);
#define bool int
#define True 1
#define False 0

typedef enum {
  OPEN_CURLY_TOKEN,
  CLOSE_CURLY_TOKEN,
  OPEN_PAREN_TOKEN,
  CLOSE_PAREN_TOKEN,
  EQUAL_TOKEN,
  SEMICOLON_TOKEN,
  LESS_TOKEN,
  GREATER_TOKEN,
  PLUS_TOKEN,
  MINUS_TOKEN,
  SLASH_TOKEN,
  ASTERISK_TOKEN,
  COMMA_TOKEN,

  EQUAL_EQUAL_TOKEN,
  LESS_EQUAL_TOKEN,
  GREATER_EQUAL_TOKEN,

  IF_TOKEN,
  ELSE_TOKEN,
  ELIF_TOKEN,
  WHILE_TOKEN,
  IDENTIFIER_TOKEN,
  INTEGER_TOKEN,
  STRING_TOKEN,
  AND_TOKEN, 
  OR_TOKEN,
  VAR_TOKEN,
  FUNC_TOKEN,
  ARR_TOKEN,

  COMMENT_TOKEN,
  END_TOKEN,
  NULL_TOKEN,
} TokenType;

static const char * const token_to_name[] = {
  [OPEN_CURLY_TOKEN] = "OPEN_CURLY",
  [CLOSE_CURLY_TOKEN] = "CLOSE_CURLY",
  [OPEN_PAREN_TOKEN] = "OPEN_PAREN",
  [CLOSE_PAREN_TOKEN] = "CLOSE_PAREN",
  [EQUAL_TOKEN] = "EQUAL",
  [LESS_EQUAL_TOKEN] = "LESS_EQUAL",
  [GREATER_EQUAL_TOKEN] = "GREATER_EQUAL",
  [LESS_TOKEN] = "LESS",
  [GREATER_TOKEN] = "GREATER",
  [PLUS_TOKEN] = "PLUS",
  [MINUS_TOKEN] = "MINUS",
  [SLASH_TOKEN] = "SLASH",
  [ASTERISK_TOKEN] = "ASTERISK",
  [IDENTIFIER_TOKEN] = "IDENTIFIER",
  [INTEGER_TOKEN] = "INTEGER",
  [STRING_TOKEN] = "STRING",
  [AND_TOKEN] = "AND",
  [OR_TOKEN] = "OR",
  [EQUAL_EQUAL_TOKEN] = "EQUAL_EQUAL",
  [IF_TOKEN] = "IF",
  [ELSE_TOKEN] = "ELSE",
  [ELIF_TOKEN] = "ELIF",
  [WHILE_TOKEN] = "WHILE",
  [COMMA_TOKEN] = "COMMA",
  [VAR_TOKEN] = "VAR",
  [ARR_TOKEN] = "ARR",
  [FUNC_TOKEN] = "FUNC",
  [SEMICOLON_TOKEN] = "SEMICOLON",
  [COMMENT_TOKEN] = "COMMENT",
  [END_TOKEN] = "END_TOKEN",
  [NULL_TOKEN] = "NULL_TOKEN"
};

static const char token_to_char[] = {
  [OPEN_CURLY_TOKEN] = '{',
  [CLOSE_CURLY_TOKEN] = '}',
  [OPEN_PAREN_TOKEN] = '(',
  [CLOSE_PAREN_TOKEN] = ')',
  [SEMICOLON_TOKEN] = ';',
  [PLUS_TOKEN] = '+',
  [MINUS_TOKEN] = '-',
  [SLASH_TOKEN] = '/',
  [ASTERISK_TOKEN] = '*',
  [COMMA_TOKEN] = ',',
};

static const char * const keyword_token_to_string[] = {
  [IF_TOKEN] = "if",
  [ELSE_TOKEN] = "else",
  [WHILE_TOKEN] = "while",
  [AND_TOKEN] = "and",
  [OR_TOKEN] = "or",
  [VAR_TOKEN] = "var",
  [FUNC_TOKEN] = "func",
  [ELIF_TOKEN] = "elif",
  [ARR_TOKEN] = "arr",
};

#define token_to_char_size sizeof(token_to_char)
#define token_to_name_size sizeof(token_to_name)/sizeof(char *)
#define keyword_token_to_string_size sizeof(keyword_token_to_string)/sizeof(char *)

typedef struct {

  TokenType type;
  const char* data;
  int data_length;
  int position;

} Token;

void token_print_data(Token token) {
  for (int i=0; i<token.data_length; i++)
    fprintf(stdout, "%c", token.data[token.position+i]);
}

char *lxr_get_token_data_as_cstring(Token token){
  // TODO: error handling
  // TODO: what should be returned in case of STRING_TOKEN --> DONE
  if (token.type == STRING_TOKEN) {
    char *string = (char *) malloc(token.data_length-1);
    memcpy(string, token.data + token.position + 1, token.data_length - 2);
    string[token.data_length - 2] = '\0';
    return string;
  }
  char *string = (char *) malloc(token.data_length+1);
  memcpy(string, token.data + token.position, token.data_length);
  string[token.data_length] = '\0';
  return string;
}

void lxr_print_token_type(TokenType token_type){
  fprintf(stdout, "%s", token_to_name[token_type]);
}

void lxr_print_token(Token token){

  fprintf(stdout, "%-14s", token_to_name[token.type]);

  if (token.data_length > 0){
    fprintf(stdout, " data: ");
    token_print_data(token);
  }
}

typedef struct {
  const char *data;
  unsigned int current;
  unsigned int data_length;
} Lexer;

#define lxr_current_char(lexer) lexer->data[lexer->current]
#define lxr_increment_current(lexer) ++lexer->current;

void lxr_dump_lexer(Lexer *lexer) {
  fprintf(stdout, "current: %d, current_char: %c\n", lexer->current, lexer->data[lexer->current]);
  fprintf(stdout, "data_length: %d\n", lexer->data_length);
  fprintf(stdout, "code: \n%s", lexer->data);
  fprintf(stdout, "\n");
}

bool is_whitespace(char c) {
  return c == ' ' || c == '\n' || c == '\t';
}

bool is_starter_identifier_char(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= '>') || c == '_';
}

bool is_identifier_char(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= '>') || (c >= '0' && c <= '9') || c == '_';
}

bool is_integer_char(char c) {
  return (c >= '0' && c <= '9');
}

bool is_final_char(char c) {
  return c == '\0' || c == 26;
}

int eat_whitespace(Lexer *lexer) {
  int eaten = 0;
  while (is_whitespace(lxr_current_char(lexer))) {
    ++eaten;
    lxr_increment_current(lexer);
  }
  return eaten;
}

TokenType lxr_get_keyword_token(const char *data, int start, unsigned int length) {
  for (unsigned int i=0; i < keyword_token_to_string_size; i++) {
    if (keyword_token_to_string[i] != NULL && strlen(keyword_token_to_string[i]) == length) {
      bool token_found=True;
      for (unsigned int j=0; j<length; j++) {
        if (data[start+j] != keyword_token_to_string[i][j]) {
          token_found=False;
          break;
        }
      }
      if (token_found) 
        return (TokenType) i;
    }
  }
  return IDENTIFIER_TOKEN;
}

Lexer lxr_init(const char *data) {

  Lexer lexer;
  lexer.data = data;
  lexer.data_length = strlen(data);
  lexer.current = 0;

  return lexer;
}

bool lxr_check_one_char_token(Lexer *lexer, Token *token) {
  for (unsigned int i=0; i < token_to_char_size; i++) {
    if (token_to_char[i] != 0 && lxr_current_char(lexer) == token_to_char[i]) {
      token->type = (TokenType) i;
      token->data_length = 1;
      token->position = lexer->current;
      return True;
    }
  }
  return False;
}

#define lxr_ambiguous_one_or_two_chars_token(first_char, first_token, secnd_char, secnd_token) \
else if (lxr_current_char(lexer) == first_char) { \
  lxr_increment_current(lexer); \
  if (lxr_current_char(lexer) == secnd_char) { \
    token.type = secnd_token; \
    token.position = lexer->current-1; \
    token.data_length = 2; \
  } else { \
    token.type = first_token; \
    token.position = lexer->current-1; \
    token.data_length = 1; \
  } \
  lxr_increment_current(lexer); \
}   \

Token lxr_next_token(Lexer *lexer) {

  eat_whitespace(lexer);
  Token token;
  // int eaten = 0;
  token.data = lexer->data;
  token.type = NULL_TOKEN;
  token.position = 0;
  token.data_length = 0;

  if (lxr_check_one_char_token(lexer, &token)) {
    lxr_increment_current(lexer);
  } 
  lxr_ambiguous_one_or_two_chars_token('<', LESS_TOKEN, '=', LESS_EQUAL_TOKEN)
  lxr_ambiguous_one_or_two_chars_token('>', GREATER_TOKEN, '=', GREATER_EQUAL_TOKEN)
  lxr_ambiguous_one_or_two_chars_token('=', EQUAL_TOKEN, '=', EQUAL_EQUAL_TOKEN)
  else if (is_starter_identifier_char(lxr_current_char(lexer))) {
    int starting_position = lexer->current;
    int id_name_length = 1;
    lxr_increment_current(lexer);

    while(is_identifier_char(lxr_current_char(lexer))) {
      ++id_name_length;
      lxr_increment_current(lexer);
    }
    token.type = lxr_get_keyword_token(lexer->data, starting_position, id_name_length);
    token.position = starting_position;
    token.data_length = id_name_length;

  }
  else if (is_integer_char(lxr_current_char(lexer))) {
    int starting_position = lexer->current;
    int id_name_length = 1;
    lxr_increment_current(lexer);

    while(is_integer_char(lxr_current_char(lexer))) {
      ++id_name_length;
      lxr_increment_current(lexer);
    }
    token.type = INTEGER_TOKEN;
    token.position = starting_position;
    token.data_length = id_name_length;
  }
  else if (lxr_current_char(lexer) == '#') {

    token.type = COMMENT_TOKEN;
    token.position = lexer->current;
    token.data_length = 1;

    lxr_increment_current(lexer);
    while (lxr_current_char(lexer) != '\n') {
      ++token.data_length;
      lxr_increment_current(lexer);
    }
  } 
  else if (lxr_current_char(lexer) == '"') {
    token.type = STRING_TOKEN;
    token.position = lexer->current;
    token.data_length = 1;

    lxr_increment_current(lexer);
    while (lxr_current_char(lexer) != '"') {
      ++token.data_length;
      lxr_increment_current(lexer);
    }
    ++token.data_length;
    lxr_increment_current(lexer);
  }
  else if (is_final_char(lxr_current_char(lexer))) {
    token.type = END_TOKEN;
    token.position = lexer->current;
    token.data_length = 1;
  } 
  else {
    fprintf(stdout, "ERROR: no possible token\n");
    lxr_increment_current(lexer);
  }

  return token;
}

#endif // end LXR_HEADER