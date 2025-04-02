#ifndef AST
#define AST

#include "allocators.h"
#include "types.h"

typedef dynamic_string dyn_str;

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
//ast->else_branch = else_branch_ast;

struct _ast {
  union {
    //
    struct {  };
    struct { void* type; dyn_str name; struct _ast* value; }; // var_decl;
  };
};

typedef struct _ast* ast;

ast* make_ast(allocator alloc);

#endif // !AST
