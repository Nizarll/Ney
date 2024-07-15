#include "libs/error.h"
#include "libs/lexer.h"
#include "libs/parser.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef FILE fl;

bool strend_width(char *string, char *end) {
  size_t len_beg = strlen(string);
  size_t len_end = strlen(end);
  if (len_end > len_beg)
    return false;
  if (memcmp(string + (len_beg - len_end), end, len_end) == 0)
    return true;
  return false;
}

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
        err(EXIT_FAILURE, "Compilation failure: Heap Overflow!");
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

void handle_args(char **args, int len) {
  char *file_path = args[1];
  if (!(strend_width(file_path, ".ney")))
    err(EXIT_FAILURE, "file format is not supported !");
  fl *file = fopen(file_path, "r");
  if (!file)
    err(EXIT_FAILURE, "specified file path does not lead to a correct file");
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *f_content = malloc(size + 1);
  fread(f_content, 1, size, file);
  Lexem lexem = generate_lexem(f_content);
  Symbol symbols[256];
  Ns* nsp = ns_new((Ns){.symbols = symbols, .sym_len = 256});
  Parser *p = parser_new((Parser){.nsp = nsp,.lexem = &lexem});
  parser_parse(p);
  // parser_dump_expr(node);
  deconstruct_lexem(&lexem);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    err(EXIT_FAILURE, "no file was passed !");
  } else
    handle_args(argv, argc);
  destroy_heaps();
  return 0;
}
