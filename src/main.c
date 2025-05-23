#include <stdio.h>
#include "parser.h"
#include "allocators.h"
#include "lexer.h"

#define NEWLINE "\n"

char content[] =
"let i = 0;"
"if (1) { } else if(0) { } else { }"
;
void print_token(token* tok)
{
  if (not tok or not tok->view) return;
  printf("%.*s %.*s\n", tok->span.len, tok->view, TokenKindStrings[tok->kind].len,
            TokenKindStrings[tok->kind].ptr);
}

int main() {
  auto arena_alloc = arena_init(16 * 1024);
  auto allc = arena_allocator_init(&arena_alloc);
  parser token_parser = {
    .alloc = allc,
    .i = 0
  };
  char* characters = allocator_alloc(&allc, 4);
  lexer _lexer = { 0 };

  auto tokens = lexer_lex(&_lexer, string(content));
  foreach(token, tokens) print_token(token);

  ast_list asts = parser_parse(&token_parser, &tokens);
}
