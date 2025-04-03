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

union _ast {
  //
  struct {  };
  struct { void* type; token name; union _ast* value; }; // var_decl;
};

typedef union _ast ast;

ast* make_ast(allocator alloc);

#endif // !AST
