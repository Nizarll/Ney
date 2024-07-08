#include "libs/lexer.h"
#include "libs/parser.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

Lexem generate_lexem(const char *text) {
  Lexer lexer = lexer_init(text, strlen(text));
  Lexem lexem;
  lexem.len = 0;
  lexem.tokens = (struct Token *)malloc(5 * sizeof(struct Token));
  lexem.size = 5;
  int curr_index = 0;
  lexem.tokens[curr_index] = lexer_next(&lexer);
  while (lexem.tokens[curr_index].type != TOKEN_EOF) {
    if (curr_index == lexem.size - 1) {
      lexem.size += 256;
      lexem.tokens = (struct Token *)realloc(lexem.tokens,
                                             lexem.size * sizeof(struct Token));
      if (lexem.tokens == NULL)
        NEY_ERR("Compilation failure: Heap Overflow!");
    }
    curr_index += 1;
    lexem.len += 1;
    lexem.tokens[curr_index] = lexer_next(&lexer);
  }
  for (int i = 0; i < lexem.len; i++) {
    printf(ANSI_COLOR_BRIGHT_GREEN "Token: " ANSI_COLOR_RESET "%.*s %s\n",
           lexem.tokens[i].value_len, lexem.tokens[i].value,
           get_token_name(lexem.tokens[i].type));
  }
  return lexem;
}

void deconstruct_lexem(Lexem *lexem) { free(lexem->tokens); }
void free_ast(Ast *ast) {
  if (ast->variant == expr && ast->rhs) {
    free_ast(ast->lhs);
  } 
  if (ast->variant == expr && ast->rhs) {
    free_ast(ast->rhs);
  }
  free(ast);
}

int main(void) {
  const char *text_to_compile = "string s = \"hello\"";
  Lexem lexem = generate_lexem(text_to_compile);
  Parser *p = parser_new((Parser){.lexem = &lexem});
  parser_parse(p);
  //parser_dump_expr(node);
  deconstruct_lexem(&lexem);
  free(p);
  return 0;
}
