#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "log.h"
#include "utils.h"
#include <setjmp.h>
#include <string.h>

#define PARSER_JMP_BACK(parser, ...)                      \
jmp_buf* err_ctx = err_ctx_back_ptr(&parser->err_stack);   \
if (err_ctx == nullptr)                                     \
  ney_err("did not find any error context to jump to");      \
  ney_log(__VA_ARGS__);\
longjmp(*err_ctx, 1)

#define PARSER_CONSUME_OR_ERR(parser, list, tkind, ...)  \
{                                                         \
  if (parser_current_tkind(parser, list) != tkind) {       \
    PARSER_JMP_BACK(parser, __VA_ARGS__);                   \
  }                                                          \
  _consume();                                                 \
}

#define PARSER_CONSUME(token, parser, list)  \
{                                             \
  token = parser_current_tok(parser, list);    \
  _consume();                                   \
}

#define _current_tok() parser_current_tok(parser, tokens)
#define _current_tkind() parser_current_tkind(parser, tokens)
#define _is_tok(kind) parser_is_tok(parser, tokens, kind)
#define _consume() parser_inc(parser)
#define _peek(...) (tokens->tokens[parser->i __VA_OPT__(+ __VA_ARGS__)])
#define _peek_kind(k, ...) (_peek(__VA_ARGS__).kind == k)

#define NEY_UNREACHABLE() ney_err("unreachable at %s, line: %d func: %s", __FILE__, __LINE__, __PRETTY_FUNCTION__)

// Inline prototypes
static __always_inline token parser_current_tok(struct _parser* parser, token_list* list);
static __always_inline token_kind parser_current_tkind(struct _parser* parser, token_list* list);
static __always_inline bool parser_is_tok(struct _parser* parser, token_list* list, token_kind kind);
static __always_inline bool parser_match(struct _parser* parser, token_list* list, any_string str);
static __always_inline bool parser_on_safepoint(struct _parser* parser, token_list* list);
static void parser_sync_to_safepoint(struct _parser* parser, token_list* list);
static __always_inline void parser_inc(struct _parser* parser);

static ast* parser_parse_stmt(struct _parser* parser, token_list* tokens);
static ast* parser_parse_expr(struct _parser* parser, token_list* tokens);
static ast* parser_parse_term(struct _parser* parser, token_list* tokens);
static ast* parser_parse_factor(struct _parser* parser, token_list* tokens);
static ast* parser_parse_var_decl(struct _parser* parser, token_list* tokens);
static ast* parser_parse_if_stmt(struct _parser* parser, token_list* tokens);
static ast* parser_parse_block(struct _parser* parser, token_list* tokens);

//-------------------- Main parsing function

ast_list parser_parse(struct _parser* parser, token_list* tokens)
{
  ast_list asts;
  LIST_INIT(&asts, ast_list);

  while (parser->i < tokens->occupied && _current_tkind() != END_OF_FILE) {
    jmp_buf* err_ctx = push_err_ctx(&parser->err_stack);
    if (err_ctx == nullptr) {
      ney_err("failed to initialize error ctx");
      break;
    }

    if (setjmp(*err_ctx) > 0) {
      // If error occurred, sync to nearest safe parsing point
      parser_sync_to_safepoint(parser, tokens);
    } else {
      ast* stmt = parser_parse_stmt(parser, tokens);
      if (stmt) {
        LIST_PUSH(&asts, stmt);
      }
    }

    pop_err_ctx(&parser->err_stack);
  }
  free(parser->err_stack.buffers);
  return asts;
}

static ast* parser_parse_stmt(struct _parser* parser, token_list* tokens)
{
  if (parser_match(parser, tokens, string("let"))) {
    ney_log("found a let");
    return parser_parse_var_decl(parser, tokens);
  }

  if (parser_match(parser, tokens, string("if"))) {
    return parser_parse_if_stmt(parser, tokens);
  }

  if (_is_tok(OPENCURLY)) {
    return parser_parse_block(parser, tokens);
  }

 
  NEY_UNREACHABLE();
  return nullptr;
}

static ast* parser_parse_var_decl(struct _parser* parser, token_list* tokens)
{
  _consume();

  token name = _current_tok();
  _consume();

  ast* initializer = nullptr;

  if (_is_tok(EQUAL)) {
    _consume();
    initializer = parser_parse_expr(parser, tokens);
    if (!initializer) {
      PARSER_JMP_BACK(parser, "Expected expression after '=' in variable declaration");
    }
  }

  ney_log("current tkind: %s", TokenKindStrings[_current_tkind()].ptr);

  PARSER_CONSUME_OR_ERR(parser, tokens, SEMICOL, "Expected semicolon at line end");
  return make_var_decl_ast(parser->alloc, name, initializer);
}

static ast* parser_parse_if_stmt(struct _parser* parser, token_list* tokens)
{
  _consume();

  ney_log("current tkind: %s", TokenKindStrings[_current_tkind()].ptr);
  PARSER_CONSUME_OR_ERR(parser, tokens, OPENPAREN, "Expected '(' after 'if'");

  ney_log("current tkind: %s", TokenKindStrings[_current_tkind()].ptr);
  ast* condition = parser_parse_expr(parser, tokens);
  if (!condition) {
    ney_log("not a condition");
    PARSER_JMP_BACK(parser, "Expected condition in if statement");
  }

  ney_log("current tkind: %s", TokenKindStrings[_current_tkind()].ptr);

  PARSER_CONSUME_OR_ERR(parser, tokens, CLOSEPAREN, "Expected ')' after if condition");

  ast* then_branch = parser_parse_stmt(parser, tokens);
  if (!then_branch) {
    PARSER_JMP_BACK(parser, "Expected statement after if condition");
  }

  ast* root_if = make_if_ast(parser->alloc, condition, then_branch, nullptr);
  ast* current_if = root_if;
  while (parser_match(parser, tokens, string("else"))) {
    _consume();
    if (parser_match(parser, tokens, string("if"))) {
      _consume();
      
      PARSER_CONSUME_OR_ERR(parser, tokens, OPENPAREN, "Expected '(' after 'if'");
      ast* else_if_condition = parser_parse_expr(parser, tokens);
      if (!else_if_condition) {
        PARSER_JMP_BACK(parser, "Expected condition in else if statement");
      }
      PARSER_CONSUME_OR_ERR(parser, tokens, CLOSEPAREN, "Expected ')' after else if condition");
      ast* else_if_then = parser_parse_stmt(parser, tokens);
      if (!else_if_then) {
        PARSER_JMP_BACK(parser, "Expected statement after else if condition");
      }
      
      ast* else_if_node = make_if_ast(parser->alloc, else_if_condition, else_if_then, nullptr);
      current_if->if_stmt.else_branch = else_if_node;
      current_if = else_if_node;
      
    } else {
      ast* else_stmt = parser_parse_stmt(parser, tokens);
      if (!else_stmt) {
        PARSER_JMP_BACK(parser, "Expected statement after 'else'");
      }
      current_if->if_stmt.else_branch = else_stmt;
      break;
    }
  }

  return root_if;
}

static ast* parser_parse_block(struct _parser* parser, token_list* tokens)
{
  PARSER_CONSUME_OR_ERR(parser, tokens, OPENCURLY, "Expected '{'");

  ast* block = make_block_ast(parser->alloc);

  while (!_is_tok(CLOSECURLY) && !_is_tok(END_OF_FILE)) {
    ast* stmt = parser_parse_stmt(parser, tokens);
    if (stmt) {
      LIST_PUSH(&block->block.statements, stmt);
    }
  }

  PARSER_CONSUME_OR_ERR(parser, tokens, CLOSECURLY, "Expected '}'");
  return block;
}

static ast* parser_parse_expr(struct _parser* parser, token_list* tokens)
{
  ast* node = parser_parse_term(parser, tokens);

  while (!_is_tok(END_OF_FILE) && (_is_tok(PLUS) || _is_tok(MINUS))) {
    token op_tok;
    PARSER_CONSUME(op_tok, parser, tokens);

    ast* right = parser_parse_term(parser, tokens);
    if (!right) {
      PARSER_JMP_BACK(parser, "Expected expression after binary operator");
    }

    node = make_binop_ast(parser->alloc, node, op_tok, right);
  }

  return node;
}

static ast* parser_parse_term(struct _parser* parser, token_list* tokens)
{
  ast* node = parser_parse_factor(parser, tokens);

  while (!_is_tok(END_OF_FILE) && (_is_tok(MUL) || _is_tok(DIV))) {
    token op_tok;
    PARSER_CONSUME(op_tok, parser, tokens);

    ast* right = parser_parse_factor(parser, tokens);
    if (!right) {
      PARSER_JMP_BACK(parser, "Expected expression after binary operator");
    }

    node = make_binop_ast(parser->alloc, node, op_tok, right);
  }

  return node;
}

static ast* parser_parse_factor(struct _parser* parser, token_list* tokens)
{
  if (_is_tok(END_OF_FILE)) {
    return nullptr;
  }

  if (_is_tok(OPENPAREN)) {
    _consume(); // consume '('
    ast* expr = parser_parse_expr(parser, tokens);
    PARSER_CONSUME_OR_ERR(parser, tokens, CLOSEPAREN, "Expected ')' after expression");
    return expr;
  }

  if (_is_tok(IDENTIFIER) && _peek_kind(OPENPAREN, 1)) {
    token func_name_tok = _current_tok();
    _consume(); // consume function name
    _consume(); // consume '('

    dynamic_string func_name = {
      (char*)func_name_tok.view,
      func_name_tok.span.len
    };
    ast* func_call = make_func_call_ast(parser->alloc, func_name);

    while (!_is_tok(CLOSEPAREN) && !_is_tok(END_OF_FILE)) {
      ast* arg = parser_parse_expr(parser, tokens);
      if (arg) {
        LIST_PUSH(&func_call->func_call.args, arg);
      }

      if (_is_tok(COMMA)) {
        _consume();
      } else if (!_is_tok(CLOSEPAREN)) {
        PARSER_JMP_BACK(parser, "Expected ',' or ')' in function arguments");
      }
    }

    PARSER_CONSUME_OR_ERR(parser, tokens, CLOSEPAREN, "Expected ')' after function arguments");
    return func_call;
  }

  if (_is_tok(IDENTIFIER)) {
    return make_identifier_ast(parser->alloc, _current_tok());
    _consume();
  }

  if (_is_tok(NUMLIT) || _is_tok(STR)) {
    ast* literal = make_literal_ast(parser->alloc, _current_tok());
    _consume();
    return literal;
  }

  if (_is_tok(MINUS) || _is_tok(PLUS) || _is_tok(NOT)) {
    token op_tok;
    PARSER_CONSUME(op_tok, parser, tokens);

    ast* operand = parser_parse_factor(parser, tokens);
    if (!operand) {
      PARSER_JMP_BACK(parser, "Expected expression after unary operator");
    }

    ast* unary = make_ast(parser->alloc, AST_UNARY);
    unary->unary.operator = op_tok;
    unary->unary.operand = operand;
    return unary;
  }

  PARSER_JMP_BACK(parser, "Unexpected token in expression");
  return nullptr;
}

//-------------------- Error context functions (keeping your existing logic)

void pop_err_ctx(err_context_stack* err_stack)
{
  if (err_stack->cursor > 0) {
    err_stack->cursor--;
  }
}

jmp_buf* push_err_ctx(err_context_stack* err_stack)
{
  if (err_stack->cursor == 0) {
    err_stack->total = LIST_BASE_SIZE;
    err_stack->cursor = 0;
    err_stack->buffers = malloc(err_stack->total * sizeof(jmp_buf));
  }
  else if (err_stack->cursor >= err_stack->total) {
    err_stack->total = err_stack->total * LIST_REALLOC_RATIO;
    err_stack->buffers = realloc(err_stack->buffers, err_stack->total * sizeof(jmp_buf));
  }

  if (err_stack->buffers == nullptr) {
    ney_err("err_ctx allocation failure consider getting more ram...");
    return nullptr;
  }

  return &err_stack->buffers[err_stack->cursor++];
}

jmp_buf* err_ctx_back_ptr(err_context_stack* err_stack)
{
  if (err_stack->cursor == 0) {
    return nullptr;
  }
  return &err_stack->buffers[err_stack->cursor - 1];
}

//-------------------- Inline functions (keeping your existing implementations)

static __always_inline token parser_current_tok(struct _parser* parser, token_list* list)
{
#ifdef DEBUG
  if (parser->i >= list->occupied) {
    ney_err("parser attempted to index out of bounds token list");
  }
#endif
  return list->tokens[parser->i];
}

static __always_inline token_kind parser_current_tkind(struct _parser* parser, token_list* list)
{
  return parser_current_tok(parser, list).kind;
}

static __always_inline bool parser_is_tok(struct _parser* parser, token_list* list, token_kind kind)
{
  return (parser_current_tkind(parser, list) == kind);
}

static __always_inline bool parser_match(struct _parser* parser, token_list* list, any_string str)
{
  token current = parser_current_tok(parser, list);
  return strncmp(current.view, str.ptr, str.len - 1) == 0;
}

static __always_inline bool parser_on_safepoint(struct _parser* parser, token_list* list)
{
  return (
    parser_match(parser, list, string("let"))     ||
    parser_match(parser, list, string("struct"))  ||
    parser_match(parser, list, string("type"))    ||
    parser_match(parser, list, string("enum"))    ||
    parser_match(parser, list, string("const"))   ||
    parser_match(parser, list, string("fn"))
  );
}

static __always_inline void parser_inc(struct _parser* parser)
{
  parser->i++;
}

static void parser_sync_to_safepoint(struct _parser* parser, token_list* list)
{
  while (parser->i < list->occupied && !parser_on_safepoint(parser, list)) {
    _consume();
  }
}
