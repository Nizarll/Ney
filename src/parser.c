#include "../libs/parser.h"
#include <stdarg.h>

static char *types[] =
{
  "i8", "i16", "i32", "i64",
  "u8", "u16", "u32", "u64",
  "f16", "f32", "f64",
  "string",
};

#define type_len                            \
  (sizeof(types) / sizeof(types[0]))

static int lvl = 0;
void err(int status, const char *str)
{
  printf("error: %s", str);
  exit(status);
}

bool is_tok(Parser *p, TokenKind kind)
{
  return p->lexem->tokens[p->cursor].type == kind;
}

bool is_tok_at(Parser *p, size_t index, TokenKind kind)
{
  return p->lexem->tokens[index].type == kind;
}

bool is_type(char* str, size_t len)
{
  bool found = false;
  for (int i = 0; i < type_len; i++) {
    if (strncmp(str, types[i], len) == 0) {
      found = true;
      break;
    }
  }
  return found;
}

bool is_add_or_sub_tok(Parser *p)
{
  return p->lexem->tokens[p->cursor].value[0] == '+' ||
         p->lexem->tokens[p->cursor].value[0] == '-';
}

bool is_mul_or_div_tok(Parser *p)
{
  return p->lexem->tokens[p->cursor].value[0] == '*' ||
         p->lexem->tokens[p->cursor].value[0] == '/';
}

void get_word_at(Parser* p, size_t i, char* buff, size_t len) 
{
  if (buff == null) {
    err(EXIT_FAILURE, "cannot put word onto a null buffer !");
  }
  if (len < p->lexem->tokens[p->cursor - i].value_len) {
    err(EXIT_FAILURE, "buffer size cannot be smaller than the token word length!");
  }
  snprintf(buff, len, "%.*s",
           p->lexem->tokens[i].value_len,
           p->lexem->tokens[i].value);
}

void parser_eat(Parser *p, TokenKind kind) {
  if (p->lexem->tokens[p->cursor].type == kind)
    p->cursor++;
}
void parser_eat_or_err(Parser *p, TokenKind kind) {
  if (p->lexem->tokens[p->cursor].type == kind)
    p->cursor++;
  else
    err(EXIT_FAILURE, "error in parser\n"); // TODO: add proper errors
}
Parser *parser_new(Parser p)
{
  Parser *parser = malloc(sizeof(Parser));
  memcpy(parser, &p, sizeof(Parser));
  parser->cursor = 0;
  parser->node = 0;
  parser->line = 0;
  parser->bol = 0;
  return parser;
}

Ast *ast_new(Ast a)
{
  if (a.variant >= var_len)
    err(EXIT_FAILURE, "ast constructor error: unknown ast kind!\n");
  Ast *ast = malloc(sizeof(Ast));
  memcpy(ast, &a, sizeof(Ast));
  return ast;
}

void dump_ast(Ast *node)
{
  if (node == null) {
    printf("Ast Node\n\tvalue:null\n");
    return;
  }
  for (int i = 0; i < lvl; i++)
    printf("\t");
  printf("Ast Node:\n");
  for (int i = 0; i < lvl; i++)
    printf("\t");
  printf("variant: %s, "
         "value: %.*s\n",
         get_ast_node_name(node), node->tok.value_len, node->tok.value);
}

void parser_dump_expr(Ast *node)
{
  if (node == null)
    return;
  dump_ast(node);
  if (node->variant == expr) {
    if (node->lhs) {
      lvl += 2;
      for (int i = 0; i < lvl - 1; i++)
        printf("\t");
      printf("left:\n");
      parser_dump_expr(node->lhs);
    }
    if (node->rhs) {
      lvl += 2;
      for (int i = 0; i < lvl - 1; i++)
        printf("\t");
      printf("right:\n");
      parser_dump_expr(node->lhs);
    }
  } else {
    lvl = 0;
  }
}

char *get_ast_node_name(Ast *node)
{
  switch (node->variant) {
  case lit:
    return "LITERAL";
  case expr:
    return "EXPRESSION";
  default:
    return "NOT FOUND";
  }
}

void parser_parse(Parser *p)
{ 
  parser_parse_decl(p);
}

Ast *parser_parse_factor(Parser *p)
{
  Ast *node = null;
  Token tok = p->lexem->tokens[p->cursor];
  parser_eat(p, p->lexem->tokens[p->cursor].type);
  if (tok.type == TOKEN_NUMBER) {
    node = ast_new((Ast){
        .variant = lit,
        .tok = tok,
    });
  } else if (tok.type == TOKEN_PARENTHESIS_OPEN) {
    node = parser_parse_expr(p);
    parser_eat(p, TOKEN_PARENTHESIS_CLOSE);
  }
  return node;
}

Ast *parser_parse_term(Parser *p)
{
  Ast *curr = parser_parse_factor(p);
  dump_ast(curr);
  Token tok = p->lexem->tokens[p->cursor];
  parser_eat(p, p->lexem->tokens[p->cursor].type);
  Ast *node = null;
  while (p->cursor < p->lexem->len && is_mul_or_div_tok(p)) {
    Token tok = p->lexem->tokens[p->cursor];
    parser_eat(p, p->lexem->tokens[p->cursor].type);
    node = ast_new((Ast){
        .lhs = curr,
        .rhs = parser_parse_expr(p),
        .variant = expr,
        .tok = tok,
    });
  }
  return node;
} 

Ast *parser_parse_expr(Parser *p)
{
  Ast *node = parser_parse_factor(p);
  while (p->cursor < p->lexem->len && is_add_or_sub_tok(p)) {
    Token tok = p->lexem->tokens[p->cursor];
    parser_eat(p, p->lexem->tokens[p->cursor].type);
    Ast *rhs = parser_parse_expr(p);
    node = ast_new((Ast){
        .variant = expr,
        .lhs = node,
        .rhs = rhs,
        .tok = tok,
    });
  }
  return node;
}

Ast* parser_parse_decl(Parser *p) {
  if (is_tok(p, TOKEN_EQUAL)) {
    Ast *node = null;
    if (p->cursor >= 2 && is_tok_at(<D-V>p, p->cursor - 2, TOKEN_SYMBOL)) {
      size_t len = p->lexem->tokens[p->cursor - 2].value_len + 1; // >>> vartype varname = value
      char type[len];
      get_word_at(p, p->cursor - 2, type, len);
      if (!is_type(type, len)) {
        err(EXIT_FAILURE, "declaration needs to have a proper type ! ");
      }
      char var_len = p->lexem->tokens[p->cursor - 1].value_len + 1; // add 1 for null terminator
      char var_name[var_len];
      get_word_at(p , p->cursor - 1, var_name, var_len);
    }
    return node;
  } else {
    if (p->cursor < p->lexem->len) {
      parser_eat(p, p->lexem->tokens[p->cursor].type);
      parser_parse_decl(p);
    }
  }
  return null;
}
