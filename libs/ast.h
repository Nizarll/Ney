#ifndef AST_H
#define AST_H

#include "allocators.h"
#include "utils.h"
#include "lexer.h"

#define AST_KIND($)  \
$(NONE),              \
$(BINOP),              \
$(UNARY),               \
$(LITERAL),              \
$(IDENTIFIER),            \
$(FUNC_CALL),              \
$(VAR_DECL),                \
$(IF_STMT),                  \
$(BLOCK_STMT),                \
$(EXPR_STMT),                  \
$(ASSIGN_STMT)

#define _CAT(A, B) A ## B
#define CAT(A, B) _CAT(A, B)
#define ENUMIZE_AST(A) CAT(AST_, A)

enum ast_kind {
  AST_KIND(ENUMIZE_AST),
  AST_TYPES_MAX
};

typedef struct _ast ast;
typedef struct _ast_list ast_list;
typedef enum ast_kind ast_kind;

struct _ast_list {
  ast** items;
  usz occupied;
  usz total;
};

struct _ast {
  ast_kind kind;

  union {
    struct {
      ast* left;
      ast* right;
      token operator;
    } binop;

    struct {
      ast* operand;
      token operator;
    } unary;

    struct {
      token tok;
    } literal;

    struct {
      token name;
      ast* initializer;
      void* type_info;
    } var_decl;

    struct {
      dynamic_string name;
      ast_list args;
    } func_call;

    struct {
      ast* condition;
      ast* then_branch;
      ast* else_branch;
    } if_stmt;

    struct {
      ast_list statements;
    } block;

    struct {
      ast* expression;
    } expr_stmt;

    struct {
      ast* target;
      ast* value;
    } assign;
  };
};

ast* make_ast(allocator alloc, ast_kind kind);

#define make_binop_ast(a, l, o, r)      \
({ ast* node = make_ast(a, AST_BINOP);   \
  node->binop.left = l;                   \
  node->binop.operator = o;                \
  node->binop.right = r;                    \
  node; })

#define make_literal_ast(a, t)            \
({ ast* node = make_ast(a, AST_LITERAL);   \
  node->literal.tok = t;                    \
  node; })

#define make_identifier_ast(a, t)            \
({ ast* node = make_ast(a, AST_IDENTIFIER);   \
  node->literal.tok = t;                       \
  node; })

#define make_var_decl_ast(a, n, i)         \
({ ast* node = make_ast(a, AST_VAR_DECL);   \
  node->var_decl.name = n;                   \
  node->var_decl.initializer = i;             \
  node->var_decl.type_info = nullptr;          \
  node; })

#define make_if_ast(a, c, tb, eb)         \
({ ast* node = make_ast(a, AST_IF_STMT);   \
  node->if_stmt.condition = c;              \
  node->if_stmt.then_branch = tb;            \
  node->if_stmt.else_branch = eb;             \
  node; })

#define make_block_ast(a)                    \
({ ast* node = make_ast(a, AST_BLOCK_STMT);   \
  node->block.statements.items = nullptr;      \
  node->block.statements.occupied = 0;          \
  node->block.statements.total = 0;              \
  node; })

#define make_func_call_ast(a, ns)           \
({ ast* node = make_ast(a, AST_FUNC_CALL);   \
  node->func_call.name = ns;                  \
  node->func_call.args.items = nullptr;        \
  node->func_call.args.occupied = 0;            \
  node->func_call.args.total = 0;                \
  node; })

#undef CAT
#undef _CAT
#undef ENUMIZE_AST

#endif // AST_H
