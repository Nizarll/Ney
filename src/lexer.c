#include "lexer.h"
#include "log.h"

#include <ctype.h>
#include <string.h>

#define ONE_CHAR 1 // no magic numbers allowed
#define TWO_CHARS 2

static void token_list_init(token_list* list)
{
#define list_base_size 32
  list->tokens = ney_alloc(list_base_size * sizeof(token));
  list->occupied = 0;
  list->total = list_base_size;
  if (list->tokens == nullptr) ney_err("list allocation failure");
#undef list_base_size
}

static void token_list_push(token_list* list, token tok)
{
#define list_realloc_ratio 1.5
#ifdef debug
  if (list == nullptr) ney_err("attempted to insert token to null list");
#endif
  if (list->tokens == nullptr) {
    token_list_init(list);
  } else if (list->occupied >= list->total) {
    list->total = list->total * list_realloc_ratio;
    list->tokens = ney_realloc(list->tokens, list->total * sizeof(token));
    if (list->tokens == nullptr) ney_err("tokenlist allocation failure");
  }
  memcpy(list->tokens + list->occupied++, &tok, sizeof(token));
#undef list_realloc_ratio
}

static void __always_inline lexer_push_token(struct _lexer* lexer, token_list* list, const char* view, u32 kind, u64 size)
{
  span token_span = {lexer->col, lexer->row, size};
  token_list_push(list, (token){view + lexer->i, token_span, kind});
  lexer->i += size;
}

static char __always_inline lexer_current_char(struct _lexer* lexer, any_string view)
{
  if (lexer->i < view.len) {
    return view.ptr[lexer->i];
  }
  return '\0';
}

static char __always_inline lexer_peek(struct _lexer* lexer, any_string view, u64 offset)
{
  #define lexer_peek(lexer, view) lexer_peek(lexer, view, ONE_CHAR)
  if (lexer->i + offset < view.len) {
    return view.ptr[lexer->i + offset];
  }
  return '\0';
}

static void __always_inline lexer_inc(lexer* lexer, any_string view) {
  if (lexer_current_char(lexer, view) == '\n') {
    lexer->loc.col = 0;
    lexer->loc.row += 1;
  } else {
    lexer->loc.col += 1;
  }
  lexer->i++;
}

void lexer_lex_indent_or_numerical(struct _lexer* lexer, token_list* list, any_string view, u32 token_kind)
{
  u64 start = lexer->i;

#define current lexer_current_char(lexer, view)
  if (token_kind == IDENTIFIER)
    while (lexer->i < view.len and (isalnum(current) || current == '_')) lexer_inc(lexer, view);
  else if(token_kind == NUMLIT)
    while (lexer->i < view.len and isalnum(current)) lexer_inc(lexer, view);
  else
    ney_err("cannot lex something that is not an identifier or a numerical in lexer_elx_ident_or_numerical");
#undef current
  u64 end = lexer->i;
  lexer->i = start;
  lexer_push_token(lexer, list, view.ptr, token_kind, end - start);
}

token_list lexer_lex(struct _lexer* lexer, any_string view)
{
  if (not lexer)
    ney_err("attempted to lex on a nullptr lexer");

  token_list list = {0};
  lexer->loc = (location){0};
  lexer->i = 0;

#define current lexer_current_char(lexer, view)
  while (lexer->i < view.len) {
    switch(current) {
    case '\t': case '\v': case '\r': case '\n': case ' ':
      lexer_inc(lexer, view);
      break;
    case '\0': lexer_push_token(lexer, &list,view.ptr, END_OF_FILE, ONE_CHAR); break;
    case '+': lexer_push_token(lexer, &list, view.ptr, PLUS, ONE_CHAR); break;
    case '-': lexer_push_token(lexer, &list, view.ptr, MINUS, ONE_CHAR); break;
    case '/': lexer_push_token(lexer, &list, view.ptr, DIV, ONE_CHAR); break;
    case '*': lexer_push_token(lexer, &list, view.ptr, MUL, ONE_CHAR); break;
    case ',': lexer_push_token(lexer, &list, view.ptr, COMMA, ONE_CHAR); break;
    case ':': lexer_push_token(lexer, &list, view.ptr, COLON, ONE_CHAR); break;
    case ';': lexer_push_token(lexer, &list, view.ptr, SEMICOL, ONE_CHAR); break;
    case '{': lexer_push_token(lexer, &list, view.ptr, OPENCURLY, ONE_CHAR); break;
    case '}': lexer_push_token(lexer, &list, view.ptr, CLOSECURLY, ONE_CHAR); break;
    case '[': lexer_push_token(lexer, &list, view.ptr, OPENBRACK, ONE_CHAR); break;
    case ']': lexer_push_token(lexer, &list, view.ptr, CLOSEBRACK, ONE_CHAR); break;
    case '(': lexer_push_token(lexer, &list, view.ptr, OPENPAREN, ONE_CHAR); break;
    case ')': lexer_push_token(lexer, &list, view.ptr, CLOSEPAREN, ONE_CHAR); break;
    case '|': lexer_push_token(lexer, &list, view.ptr, PIPE, ONE_CHAR); break;
    case '.': lexer_push_token(lexer, &list, view.ptr, DOT, ONE_CHAR); break;
    case '=':
      if (lexer_peek(lexer, view) == '>') lexer_push_token(lexer, &list, view.ptr, ARROW, TWO_CHARS);
      else if (lexer_peek(lexer, view) == '=') lexer_push_token(lexer, &list, view.ptr, ISEQ, TWO_CHARS);
      else lexer_push_token(lexer, &list, view.ptr, EQUAL, ONE_CHAR);
      break;
    default:
        if (current == '_' || isalpha(current)) lexer_lex_indent_or_numerical(lexer, &list, view, IDENTIFIER);
        else if(isdigit(current)) lexer_lex_indent_or_numerical(lexer, &list, view, NUMLIT);
        else ney_err("unimplemented lexing token");
    }
  }
#undef current
  return list;
}
