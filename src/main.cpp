#include "../libs/parser.h"
#include <cstring>
#include <string>
#include <print>

int main() {
  char content[] = "u8 int";
  auto len = strlen(content);
  lexer l {
    content,
    0,
    0,
    0,
     len,
  };
  lexem lxm = l.lex();
  parser p{};
  ast_list list = p.parse_lexem(lxm);
  list.destroy();
  return 0;
}

