#ifndef LEXER_H
#define LEXER_H

#include "ansi.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEY_OKAY(msg, ...)                                                     \
  printf(ANSI_COLOR_GREEN "[✓] | " msg ANSI_COLOR_RESET "\n", ##__VA_ARGS__)
#define NEY_ERR(msg, ...)                                                      \
  do {                                                                         \
    printf(ANSI_COLOR_RED "[✘] | " msg ANSI_COLOR_RESET "\n", ##__VA_ARGS__);  \
    exit(1);                                                                   \
  } while (0)
#define NEY_WARN(msg, ...)                                                     \
  printf(ANSI_COLOR_YELLOW "[!] | " msg ANSI_COLOR_RESET "\n", ##__VA_ARGS__)

typedef enum TOKEN_TYPE {
  TOKEN_EOF = 0, // done
  TOKEN_EOL,     // to be reworked
  TOKEN_OP,
  TOKEN_GREATER,
  TOKEN_SMALLER,
  TOKEN_EQUAL, // done
  TOKEN_ISEQ,  // done
  TOKEN_CURLY_OPEN,
  TOKEN_CURLY_CLOSE,
  TOKEN_PARENTHESIS_OPEN,
  TOKEN_PARENTHESIS_CLOSE,
  TOKEN_NUMBER,
  TOKEN_SYMBOL, // done
  TOKEN_KEYWORD,
  TOKEN_CHAR,   // maybe done
  TOKEN_STRING, // done
  TOKEN_INVALID // done
} TokenKind;

typedef struct Token {
  TokenKind type;
  const char *value;
  uint8_t value_len;
} Token;

typedef struct Lexer {
  const char *content;
  uint32_t content_len;
  uint32_t cursor;
  uint32_t line;
  uint32_t bol;
} Lexer;

typedef struct Type {
  enum { t_arr, t_fn, t_int, t_float, t_string } variant;
  union {
    struct {
      struct Type *elem;
    } case_arr;
    struct {
      struct Type **args;
      struct Type *ret;
      size_t nArgs;
    } case_fn;
  };
} Type;

struct Ast {
  enum {
    lit = 0,
    binop,
    unop,
    expr,
    ident,
    string,
    number,
    if_st,
    loop
  } variant;
  Token tok;
  Type type;
  struct Ast *kid1, *kid2;
};

typedef struct Ast Ast;
typedef struct Lexem Lexem;

char *get_token_name(TokenKind token_type);
char *get_ast_node_name(Ast *node);
Lexer lexer_init(const char *content, uint32_t content_len);
Token lexer_next(Lexer *lexer);
Ast *parse(Lexem *lexem);
Ast parse_at(Ast *list, Lexem *lexem, size_t i);
#endif // LEXER_H
