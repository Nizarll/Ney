#include "../libs/parser.h"
#include <cstring>
#include <print>
#include <string>

int main() {
  char* content = "a() b() c() hello world";
  auto len = sizeof(content);
  lexer l {
    content,
    0,
    0,
    0,
    strlen(content),
  };
  lexem lxm = l.lex();
  parser p{};
  p.parse_lexem(lxm);
  return 0;
}

