#ifndef PARSER_H
#define PARSER_H

#define null (void *)0x00

#include "./lexer.h"
#include <stdbool.h>

#define DECLTYPE(x) x, ptr_##x

struct Type {
  enum {
    DECLTYPE(t_arr),
    DECLTYPE(t_fn),
    DECLTYPE(t_i8),
    DECLTYPE(t_i16),
    DECLTYPE(t_i32),
    DECLTYPE(t_i64),
    DECLTYPE(t_i128),
    DECLTYPE(t_u8),
    DECLTYPE(t_u16),
    DECLTYPE(t_u32),
    DECLTYPE(t_u64),
    DECLTYPE(t_u128),
    DECLTYPE(t_f16),
    DECLTYPE(t_f32),
    DECLTYPE(t_f64),
    DECLTYPE(t_f128),
    DECLTYPE(t_string),
    DECLTYPE(t_char),
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
};

struct Ast {
  enum {
    lit,
    binop,
    unop,
    expr,
    decl,
    ident,
    string,
    number,
    if_st,
    loop,
    var_len
  } variant;
  Token tok;
  struct Type type;
  union {
    struct {
      struct Ast *lhs;
      struct Ast *rhs;
    };
    int val;
  };
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


struct Symbol {
  Token tok;
  struct Type type;
}; // variables declared

struct Ns {
  struct Symbol* symbols;
};

typedef struct Ns Ns;
typedef struct Ast Ast;
typedef struct Type Type;
typedef struct Symbol Symbol;
typedef struct Parser Parser;

char *get_ast_node_name(Ast *node);
Ast *parser_parse_expr(Parser *p);
Ast* parser_parse_decl(Parser *p);
Parser *parser_new(Parser p);
void parser_parse(Parser *p);
void parser_dump_expr(Ast *node);
#endif // PARSER_H
