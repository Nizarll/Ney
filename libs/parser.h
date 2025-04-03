#ifndef PARSER
#define PARSER

#include "utils.h"
#include "lexer.h"
#include "ast.h"
#include "allocators.h"

#include <setjmp.h>

struct _err_context_stack {
  jmp_buf* buffers;
  uint cursor;
  uint total;
};

struct _parser {
  allocator alloc;
  struct _err_context_stack err_stack;
  //TODO: other stuff such as namespaces and such
  u64 i;
};

struct _ast_list {
  ast** items;
  usz occupied;
  usz total;
};

typedef struct _parser parser;
typedef struct _ast_list ast_list;
typedef struct _err_context_stack err_context_stack;

ast_list parser_parse(struct _parser* parser, token_list* list);
void parser_parse_stmt(struct _parser* parser, token_list* tokens, ast_list* asts); //TODO: these should be returning an ast*
void parser_parse_var_decl(struct _parser* parser, token_list* tokens, ast_list* asts);

ast* parser_parse_sum_or_sub(struct _parser* parser, token_list* tokens);
ast* parser_parse_mul_or_div(struct _parser* parser, token_list* tokens);

void pop_err_ctx(err_context_stack* err_stack);
jmp_buf* push_err_ctx(err_context_stack* err_stack);
jmp_buf* err_ctx_back_ptr(err_context_stack* err_stack);


#endif // !PARSER
