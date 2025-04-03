#include "parser.h"
#include "lexer.h"
#include "log.h"
#include <setjmp.h>
#include <string.h>

//TODO: push error string and stuff after jumping to an err_ctx
// also add functions for addign 

#define PARSER_JMP_BACK(parser)                             \
  jmp_buf* err_ctx = err_ctx_back_ptr(&parser->err_stack);   \
   if (err_ctx == nullptr)                                    \
    ney_err("did not find any error context to jump to");      \
  longjmp(*err_ctx, 1)                                          \

#define PARSER_CONSUME_OR_ERR(parser, list, tkind, error)  \
{                                                           \
  if (parser_current_tkind(parser, list) != tkind) {         \
    PARSER_JMP_BACK(parser);                                  \
  }                                                            \
  parser_inc(parser);                                           \
}

#define PARSER_CONSUME_MATCH_OR_ERR(parser, list, tkind, match_str, error)                                   \
{                                                                                                             \
  if (parser_current_tkind(parser, list) != tkind or (not parser_match(parser, list, string(match_str)))) {    \
    PARSER_JMP_BACK(parser);                                                                                    \
  }                                                                                                              \
  parser_inc(parser);                                                                                             \
}

#define PARSER_EXPECT_OR_ERR(token, parser, list, kind, error)  \
{                                                                \
  if (parser_current_tkind(parser, list) != kind) {               \
    PARSER_JMP_BACK(parser);                                       \
  }                                                                 \
  token = parser_current_tok(parser, list);                          \
}

#define PARSER_EXPECT_CONSUME(token, parser, list, kind, error)  \
{                                                                 \
  PARSER_EXPECT_OR_ERR(token, parser, list, kind, error);          \
  parser_inc(parser);                                               \
}

#define NEY_UNREACHABLE() ney_err("unreachable at %s, line: %d func: %s", __FILE__, __LINE__, __PRETTY_FUNCTION__)

//-------------------- Inline Prototypes:

static __always_inline token parser_current_tok(struct _parser* parser, token_list* list);
static __always_inline token_kind parser_current_tkind(struct _parser* parser, token_list* list);
static __always_inline bool parser_match(struct _parser* parser, token_list* list, any_string str);
static __always_inline bool parser_on_safepoint(struct _parser* parser, token_list* list);
static __always_inline void parser_inc(struct _parser* parser);
static void parser_sync_to_safepoint(struct _parser* parser, token_list* list);

//-------------------- Non Inline functions:

ast_list parser_parse(struct _parser* parser, token_list* tokens)
{
  ast_list asts;
 //TODO:add use of allocators here
  #define IS_EOF(parser, tokens) parser_current_tkind(parser, tokens)

  while (parser->i < tokens->occupied && IS_EOF(parser, tokens) != EOF) {
  
    jmp_buf* err_ctx = push_err_ctx(&parser->err_stack);
    if (err_ctx == nullptr)
      ney_err("failed to initialize error ctx");

    if (setjmp(*err_ctx) > 0) // if error occured sync to nearest safe parsing point
      parser_sync_to_safepoint(parser, tokens);

    // pop the err context we established
    pop_err_ctx(&parser->err_stack);

    parser_parse_stmt(parser, tokens, &asts);
  }

  #undef IS_EOF
  
  free(parser->err_stack.buffers);
  //TODO: remove this once implementing allocator based
  //error stack
  return asts;
}

void parser_parse_var_decl(struct _parser* parser, token_list* tokens, ast_list* asts)
{
  PARSER_CONSUME_OR_ERR(parser, tokens, IDENTIFIER, "expected let in declaration statement");
  token name;

  allocator allocator = {0};
  ast* decl = make_ast(parser->alloc);

  PARSER_EXPECT_CONSUME(name, parser, tokens, IDENTIFIER, "expected a name identifier after let keyword in variable declaration");
  decl->name = name;

  LIST_PUSH(asts, ast);

  if (parser_current_tkind(parser, tokens) != EQUAL) {
    //TODO: parse for semicolon and return but im too lazy
  }
  parser_inc(parser); // we already know that there is an equal here
  parser_inc(parser);// yeah i gotta parse expressions but im too lazy TODO:
  parser_inc(parser);
}

void parser_parse_stmt(struct _parser* parser, token_list* tokens, ast_list* asts)
{
  if (parser_match(parser, tokens, string("let"))) {
    parser_parse_var_decl(parser, tokens, asts);
    return;
  }
  
  NEY_UNREACHABLE();
}

//-------------------- Error context functions (for establishing safe point and going back to safepoints):
//
void pop_err_ctx(err_context_stack* err_stack)
{
  err_stack->cursor--;
}

jmp_buf* push_err_ctx(err_context_stack* err_stack)
{//TODO: maybe switch this from dynamic memory to allocator based memory
//to reduce footprint, fragmentation and causes of program fatal crashes
//in case of allocation failures !
//
// maybe a task for futur me since im fucking lazy
  if (err_stack->cursor == 0) {
    err_stack->total = LIST_BASE_SIZE;
    err_stack->cursor = 0;
    err_stack->buffers = malloc(err_stack->total * sizeof(jmp_buf));
  }
  else if (err_stack->cursor >= err_stack->total) {
    err_stack->total = err_stack->total * LIST_REALLOC_RATIO;
    err_stack->buffers = realloc(err_stack->buffers, err_stack->total * sizeof(jmp_buf));
  }

  if (err_stack->buffers == nullptr)
    ney_err("err_ctx allocation failure consider getting more ram...");

  return err_stack->buffers + err_stack->cursor;
}

jmp_buf* err_ctx_back_ptr(err_context_stack* err_stack)
{ //TODO: bounds checking is for nerds
  return err_stack->buffers + err_stack->cursor;
}

//-------------------- Inline functions:

static __always_inline token parser_current_tok(struct _parser* parser, token_list* list)
{
#ifdef debug
  if (parser->i > list->occupied)
    ney_err("parser attempted to index out of bounds token list");
#endif
  return list->tokens[parser->i];
}

static __always_inline token_kind parser_current_tkind(struct _parser* parser, token_list* list)
{
  return parser_current_tok(parser, list).kind;
}

static __always_inline bool parser_match(struct _parser* parser, token_list* list, any_string str)
{

  return strncmp(parser_current_tok(parser, list).view, str.ptr, str.len - 1) == 0; // len - 1 since sizeof(ptr) / sizeof(ptr)[0] accounts for null terminator
}

static __always_inline bool parser_on_safepoint(struct _parser* parser, token_list* list)
{
  return (
    parser_match(parser, list, string("let"))     or
    parser_match(parser, list, string("struct"))  or
    parser_match(parser, list, string("type"))    or
    parser_match(parser, list, string("enum"))    or
    parser_match(parser, list, string("const"))   or
    parser_match(parser, list, string("fn")) //TODO replace this by actual type parsing
  );
}

static __always_inline void parser_inc(struct _parser* parser)
{
  parser->i++;
}

static void parser_sync_to_safepoint(struct _parser* parser, token_list* list)
{
  while (parser->i < list->occupied && not parser_on_safepoint(parser, list))
    parser_inc(parser);
}
