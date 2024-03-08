#include "libs/lexer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct Lexem {
  Token *tokens;
  uint32_t size;
};

struct Lexem generate_lexem(const char *text) {
  Lexer lexer = lexer_init(text, strlen(text));
  struct Lexem lexem;
  lexem.tokens = (struct Token *)malloc(5 * sizeof(struct Token));
  lexem.size = 5;
  int curr_index = 0;
  lexem.tokens[curr_index] = lexer_next(&lexer);
  while (lexem.tokens[curr_index].type != TOKEN_EOF) {
    if (curr_index == lexem.size - 1) {
      lexem.size += 1;
      lexem.tokens = (struct Token *)realloc(lexem.tokens,
                                             lexem.size * sizeof(struct Token));
      if (lexem.tokens == NULL)
        NEY_ERR("BUY MORE RAM ! XDDDDDDD");
    }
    curr_index += 1;
    lexem.tokens[curr_index] = lexer_next(&lexer);
  }
  for (int i = 0; i < lexem.size; i++) {
    printf("%.*s %s\n", lexem.tokens[i].value_len, lexem.tokens[i].value,
           get_token_name(lexem.tokens[i].type));
  }
  NEY_WARN("size of lexem is %d", lexem.size);
  return lexem;
}

void deconstruct_lexem(struct Lexem *lexem) { free(lexem->tokens); }

int main(void) {
  const char *text_to_compile =
      "import \"Hello\" \"this is a cool string + * - / "
      "yuuur listen man\"as File; hello world == = * / - +"
      "\"This is a hex value: 0xFFFA\"symbol wow 0xfff 123 0123 0xff123";
  struct Lexem lexem = generate_lexem(text_to_compile);
  deconstruct_lexem(&lexem);
  return 0;
}
