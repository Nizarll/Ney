#ifndef AST
#define AST

// we use anonymous struct so that the parent struct inherits the same properties
// example:
// struct _my_struct {
//  struct { int a; int b; };
// };
//
// struct _my_struct a;
// a.x = 0;
// a.y = 0;
//
//anonymous structs allow us to modify properties of that struct through the parent
//
//this allows us to have a better simplistic design where doing
//
//struct _ast* if_stmt = make_ast(IF_STATEMENT);
//ast->condition = other_ast;
//ast->then_branch = other_then_branch_ast;
//
//ast->else_branch = else_branch_ast;

#include "allocators.h"
#include "utils.h"
#include "lexer.h"

#define AST_KIND($)  \
$(NONE),              \
$(BINOP),              \
$(FUNC_CALL),           \
$(IF_STATEMENT),         \
$(EXPR),                  \
$(VAR_NAME),               \
$(FUNC_CALL_ARGS),          \
$(RVALUE),                   \
$(LVALUE),                    \
$(UNARY)

#define _CAT(A, B) A ## B
#define CAT(A, B) _CAT(A, B)
#define ENUMIZE_AST(A) CAT(AST_, A)

enum _ast_kind {
  AST_KIND(ENUMIZE_AST),
  AST_TYPES_MAX
};

struct _ast {
  enum _ast_kind kind;
  union {
    struct { void* type; token name; struct _ast* value; }; // var_decl;
    
    struct {
      struct _ast* left;
      struct _ast* right;
      token operator;
    }; // expression  binary operator;
    //
    struct {
      struct _ast* node; //TODO: add type member
      token tok;
    }; // expression unary operator;
      
    struct {
      struct _ast* items;
      usz occupied;
      usz total;
    }; // function arguments vector; or might be reused for other list items

    struct {
      dynamic_string func_name;
      
    }; // function call;
    
  };
};

typedef struct _ast ast;
typedef enum _ast_kind ast_kind;

#define make_ast(allocator, ...) __VA_OPT__(make_ast(allocator, __VA_ARGS__))  \
                                __NO_OPT__(__VA_ARGS__)(make_ast(allocator, 0))

ast* make_ast(allocator alloc, ast_kind kind);

#undef CAT
#undef _CAT
#undef ENUMIZE_AST
#undef AST_TYPES
#endif // !AST
