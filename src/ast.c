#include "ast.h"
#include "log.h"
#include <string.h>

ast* make_ast(allocator alloc, ast_kind kind)
{
    ast* node = malloc(sizeof(ast));
    if (!node) {
        ney_err("Failed to allocate AST node");
        return nullptr;
    }
    
    memset(node, 0, sizeof(ast));
    node->kind = kind;
    return node;
}
