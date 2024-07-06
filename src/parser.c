#include "../libs/parser.h"
#include <stdarg.h>

static int lvl = 0;

void err(int status, const char *str) {
  printf("error: %s", str);
  exit(status);
}

bool is_add_or_sub_tok(Parser *p) {
  return p->lexem->tokens[p->cursor].value[0] == '+' ||
         p->lexem->tokens[p->cursor].value[0] == '-';
}

bool is_mul_or_div_tok(Parser *p) {
  return p->lexem->tokens[p->cursor].value[0] == '*' ||
         p->lexem->tokens[p->cursor].value[0] == '/';
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

Parser *parser_new(Parser p) {
  Parser *parser = malloc(sizeof(Parser));
  memcpy(parser, &p, sizeof(Parser));
  parser->cursor = 0;
  parser->node = 0;
  parser->line = 0;
  parser->bol = 0;
  return parser;
}

Ast *ast_new(Ast a) {
  if (a.variant >= var_len)
    err(EXIT_FAILURE, "ast constructor error: unknown ast kind!\n");
  Ast *ast = malloc(sizeof(Ast));
  memcpy(ast, &a, sizeof(Ast));
  return ast;
}

void dump_ast(Ast *node) {
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

void parser_dump_expr(Ast *node) {
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

char *get_ast_node_name(Ast *node) {
  switch (node->variant) {
  case lit:
    return "LITERAL";
  case expr:
    return "EXPRESSION";
  default:
    return "NOT FOUND";
  }
}

void parser_parse(Parser *p) { parser_parse_expr(p); }

// struct Ast {
//   enum {
//     lit,
//     binop,
//     unop,
//     expr,
//     ident,
//     string,
//     number,
//     if_st,
//     loop,
//     var_len
//   } variant;
//   union {
//     struct Ast *lhs;
//     struct Ast *rhs;
//     int val;
//   };
//   Token tok;
//   Type type;
// };

Ast *parser_parse_factor(Parser *p) {
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

Ast *parser_parse_term(Parser *p) {
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

Ast *parser_parse_expr(Parser *p) {
  Ast *node = parser_parse_factor(p);
  while (p->cursor < p->lexem->len && is_add_or_sub_tok(p)) {
    Token tok = p->lexem->tokens[p->cursor];
    parser_eat(p, p->lexem->tokens[p->cursor].type);
    node = ast_new((Ast){
        .lhs = node,
        .rhs = parser_parse_expr(p),
        .variant = expr,
        .tok = tok,
    });
  }
  return node;
}
