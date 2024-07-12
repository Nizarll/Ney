#ifndef PARSER_H
#define PARSER_H

#include "./error.h"
#include "./lexer.h"
#include <stdbool.h>
#include <string.h>

#define NEY_REALLOC_STEP 128
#define TYPES($) $(i8), $(i16), $(i32), $(i64), $(i128), \
                $(u8), $(u16), $(u32), $(u128),           \
                $(f16), $(f32), $(f64), $(f128),           \
                $(str), $(char)
#define NOP($) $
#define PREFIX(B, A) B##A
#define PTR_PREFIX(T) PREFIX(ptr_,T)
#define DECLTYPE(x) _DECLTYPE(x)
#define _DECLTYPE(x) T_PREFIX(x), PTR_PREFIX(x)
#define T_PREFIX(T) PREFIX(t_,T)
#define STRING(t) #t
#define STRUCT($) (type_t) {STRING($), T_PREFIX($)}

typedef struct {
  const char *str_val;
  size_t val;
} type_t;

struct Type {
  enum {
    TYPES(DECLTYPE),
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

static char *str_types[] = {
  TYPES(STRING),
};
static type_t type_pairs[] = {
  TYPES(STRUCT),
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
      struct Ast *val;
      struct Ast *lhs;
      struct Ast *rhs;
    };
    int val;
  };
};

struct Symbol {
  Token tok;
  const char *val;
  struct Type type;
  size_t len;
};

struct Ns {
  struct Symbol *symbols;
  size_t sym_len;
  size_t sym_occ;
};

struct Parser {
  struct Ns *nsp;
  struct Ast *nodes;
  struct Lexem *lexem;
  uint32_t len;
  uint32_t node;
  uint32_t cursor; // NOTE: current token
  uint32_t line;
  uint32_t bol;
  // TODO: ADD ERRORS
};

typedef struct Ns Ns;
typedef struct Ast Ast;
typedef struct Type Type;
typedef struct Symbol Symbol;
typedef struct Parser Parser;

Parser *parser_new(Parser p);
Ns *ns_new(Ns nsp);
char *get_ast_node_name(Ast *node);
Ast *parser_parse_expr(Parser *p);
Ast *parser_parse_decl(Parser *p);
void parser_parse(Parser *p);
void parser_dump_expr(Ast *node);

#undef NOP
#undef TYPES
#undef STRUCT
#undef STRING
#undef PREFIX
#undef DECLTYPE
#undef _DECLTYPE
#undef PTR_PREFIX

#endif // PARSER_H
