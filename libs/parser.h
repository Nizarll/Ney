#ifndef PARSER_H
#define PARSER_H

#include "alloc.h"
#include "error.h"
#include "lexer.h"
#include <stdbool.h>
#include <string.h>

#define NEY_REALLOC_STEP 128
#define LIFETIMES($) $(const), $(comptime), $(static)
#define TYPES($) $(i8), $(i16), $(i32), $(i64), $(i128), \
                $(u8), $(u16), $(u32), $(u128),           \
                $(f16), $(f32), $(f64), $(f128),           \
                $(str), $(char)
#define EXPAND($) $
#define NOP($) $
#define PREFIX(B, A) B##A
#define DECLTYPE($) _DECLTYPE($)
#define _DECLTYPE($) T_PREFIX($), PTR_PREFIX($)
#define T_PREFIX($) PREFIX(t_,$)
#define PTR_PREFIX($) PREFIX(ptr_,$)
#define PTR_STRING_HELPER($$, $) STRING($$ ## $)
#define PTR_STRING($) PTR_STRING_HELPER(ptr_, $)
#define STRING($) #$
#define STRUCT($) (type_t) {STRING($), T_PREFIX($)}
#define PTR_STRUCT($) (type_t) {PTR_STRING($), PTR_PREFIX($)}
/*
  TYPES(STRUCT)
*/

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
      // struct linked_element:
      // each member of a struct declaration is linked to the next one
      // so it is easier to keep track of data in case we have a hybrid allocation
      // see libs/alloc.h
      // why hybrid allocation ? because starting off with contiguous memory is faster
      // and less annoying to work with
      struct l_mem {
        struct Type type;
        struct l_mem *next;
      };
    }case_struct;
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
      struct Ast *rhs;
      struct Ast *lhs;
    };
    struct Ast *val;
  };
};

struct Symbol {
  enum {
    LIFETIMES(T_PREFIX),
  } lft;
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

static char *str_types[] = {
  TYPES(STRING),
};
static type_t type_pairs[] = {
  TYPES(STRUCT),
  TYPES(PTR_STRUCT),
};
static type_t lft_pairs[] = {
  LIFETIMES(STRUCT),
};

Parser *parser_new(Parser p);
Ns *ns_new(Ns nsp);
char *get_ast_node_name(Ast *node);
Ast *parser_parse_expr(Parser *p);
Ast *parser_parse_assign_decl(Parser *p);
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
