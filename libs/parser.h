#ifndef PARSER_H
#define PARSER_H

#define null (void *)0x00

#include "./lexer.h"
#include <stdbool.h>

typedef struct Type {
  enum {
    t_arr,
    t_fn,
    t_int,
    t_float,
    t_string,
  } variant;
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
    lit,
    binop,
    unop,
    expr,
    ident,
    string,
    number,
    if_st,
    loop,
    var_len
  } variant;
  union {
    struct {
      struct Ast *lhs;
      struct Ast *rhs;
    };
    int val;
  };
  Token tok;
  Type type;
};

struct Parser {
  struct Ast *nodes;
  Lexem *lexem;
  uint32_t len;
  uint32_t node;
  uint32_t cursor; // NOTE: current token
  uint32_t line;
  uint32_t bol;
  // TODO: ADD ERRORS
};

typedef struct Parser Parser;
typedef struct Ast Ast;

char *get_ast_node_name(Ast *node);
Ast *parser_parse_expr(Parser *p);
Parser *parser_new(Parser p);
void parser_dump_expr(Ast *node);
#endif // PARSER_H
