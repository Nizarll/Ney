#include "libs/lexer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
int main(void) {
  const char *text_to_compile = "import \"Hello\" \"this is a cool string "
                                "yuuur listen man\"as File; hello world == =";
  Lexer l = lexer_init(text_to_compile, strlen(text_to_compile));
  Token tok;
  tok = lexer_next(&l);
  while (tok.type != TOKEN_EOF) {
    NEY_OKAY("%.*s, (%s)", (int)tok.value_len, tok.value,
             get_token_name(tok.type));
    tok = lexer_next(&l);
  }
  return 0;
}
