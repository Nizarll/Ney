#include "ast.h"

ast* make_ast(allocator alloc)
{
  return alloc.allocate(sizeof(ast), alloc.ctx);
}
