#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TODO() fprintf(stdout, "TODO: %s is not implemented\n", __FUNCTION__);
#define bool int
#define True 1
#define False 0

typedef enum token_type {
  // one char token
  OPEN_CURLY,
  CLOSE_CURLY,
  OPEN_PAREN,
  CLOSE_PAREN,
  EQUAL,
  SEMICOLON,
  LESS,
  GREATER,

  // two chars token
  EQUAL_EQUAL,
  LESS_EQUAL,
  GREATER_EQUAL,

  // more chars token
  IF,
  ELSE,
  IDENTIFIER,
  INTEGER,

  // special token
  END_TOKEN,
  NULL_TOKEN,
} token_type_t;

static const char * const token_names[] = {
  [OPEN_CURLY] = "OPEN_CURLY",
  [CLOSE_CURLY] = "CLOSE_CURLY",
  [OPEN_PAREN] = "OPEN_PAREN",
  [CLOSE_PAREN] = "CLOSE_PAREN",
  [EQUAL] = "EQUAL",
  [LESS_EQUAL] = "LESS_EQUAL",
  [GREATER_EQUAL] = "GREATER_EQUAL",
  [LESS] = "LESS",
  [GREATER] = "GREATER",
  [IDENTIFIER] = "IDENTIFIER",
  [INTEGER] = "INTEGER",
  [EQUAL_EQUAL] = "EQUAL_EQUAL",
  [IF] = "IF",
  [ELSE] = "ELSE",
  [SEMICOLON] = "SEMICOLON",
  [END_TOKEN] = "END_TOKEN",
  [NULL_TOKEN] = "NULL_TOKEN"
};

static const char token_symbols[] = {
  [OPEN_CURLY] = '{',
  [CLOSE_CURLY] = '}',
  [OPEN_PAREN] = '(',
  [CLOSE_PAREN] = ')',
  [SEMICOLON] = ';',
};

#define token_symbols_size sizeof(token_symbols)
#define token_names_size sizeof(token_symbols)/sizeof(char *)


typedef struct token {

  token_type_t type;
  const char* data;
  int data_length;
  int position;

} token_t;

void token_print_data(token_t token) {
  for (int i=0; i<token.data_length; i++)
    fprintf(stdout, "%c", token.data[token.position+i]);
}

void lxr_print_token(token_t token){

  fprintf(stdout, "%s", token_names[token.type]);

  if (token.data_length > 0){
    fprintf(stdout, ", data: ");
    token_print_data(token);
  }
}

typedef struct lxr {
  const char *data;
  int current;
  int data_length;
} lxr_t;

#define lxr_current_char(lexer) lexer->data[lexer->current]
#define lxr_increment_current(lexer) ++lexer->current;

void lxr_dump_lexer(lxr_t *lexer) {
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

int eat_whitespace(lxr_t *lexer) {
  int eaten = 0;
  while (is_whitespace(lxr_current_char(lexer))) {
    ++eaten;
    lxr_increment_current(lexer);
  }
  return eaten;
}

lxr_t lxr_init(const char *data) {

  lxr_t lexer;
  lexer.data = data;
  lexer.data_length = strlen(data);
  lexer.current = 0;

  return lexer;
}

bool lxr_check_one_char_token(lxr_t *lexer, token_t *token) {

  for (int i=0; i<token_symbols_size; i++) {
    if (token_symbols[i] != 0 && lxr_current_char(lexer) == token_symbols[i]) {
      token->type = i;
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

token_t lxr_next_token(lxr_t *lexer) {

  eat_whitespace(lexer);
  token_t token;
  int eaten = 0;
  token.data = lexer->data;
  token.type = NULL_TOKEN;
  token.position = 0;
  token.data_length = 0;

  if (lxr_check_one_char_token(lexer, &token)) {
    lxr_increment_current(lexer);
  } 
  lxr_ambiguous_one_or_two_chars_token('<', LESS, '=', LESS_EQUAL)
  lxr_ambiguous_one_or_two_chars_token('>', GREATER, '=', GREATER_EQUAL)
  lxr_ambiguous_one_or_two_chars_token('=', EQUAL, '=', EQUAL_EQUAL)
  else if (is_starter_identifier_char(lxr_current_char(lexer))) {
    int starting_position = lexer->current;
    int id_name_length = 1;
    lxr_increment_current(lexer);

    while(is_identifier_char(lxr_current_char(lexer))) {
      ++id_name_length;
      lxr_increment_current(lexer);
    }
    token.type = IDENTIFIER;
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
    token.type = INTEGER;
    token.position = starting_position;
    token.data_length = id_name_length;
  }
  else if (lxr_current_char(lexer) == '\0') {

    token.type = END_TOKEN;
    token.position = lexer->current;
    token.data_length = 1;

  } else {
    fprintf(stdout, "ERROR: no possible token\n");
    lxr_increment_current(lexer);
  }

  return token;
}