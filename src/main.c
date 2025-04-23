#include <stdio.h>
#include "parser.h"
#include "allocators.h"
#include "lexer.h"

#define NEWLINE "\n"

char content[] =
"let i = 0";

void print_token(token* tok)
{
  if (tok == nullptr || tok->view == nullptr) return;
  printf("%.*s %s\n", tok->span.len, tok->view, TokenKindStrings[tok->kind].ptr);
}

int main() {
  auto arena = arena_init(16 * 1024);
  auto allocator = arena_allocator_init(&arena);
  parser token_parser = {
    .alloc = ,
    .i = 0
  };
  char* characters = allocator_alloc(&allocator, 4);
  lexer _lexer = { 0 };

  auto tokens = lexer_lex(&_lexer, string(content));
  foreach(token, tokens) print_token(token);
  
  parser_parse(&token_parser, &tokens);
}
