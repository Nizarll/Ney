#include "ast.h"

ast* make_ast(allocator alloc, ast_kind kind)
{
  ast* node = alloc.allocate(sizeof(ast), alloc.ctx);
  node->kind = kind;
  return node;
}
