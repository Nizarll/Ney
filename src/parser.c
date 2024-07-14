#include "../libs/parser.h"

#define TYPES_LEN (sizeof(str_types) / sizeof(str_types[0]))
#define TYPES_PAIR_LEN (sizeof(type_pairs) / sizeof(type_pairs[0]))

bool is_word(char* src, char* lookup, size_t len) {
  if (strncmp(src, lookup, len) == 0)
    return true;
  return false;
}

int get_type_from_str(char *content, size_t len) {
  for (size_t i = 0; i < TYPES_PAIR_LEN; i++) {
    if (memcmp(content, type_pairs[i].str_val, len) == 0) {
      return type_pairs[i].val;
    }
  }
  return -1;
}

bool var_declared(Ns *nsp, char *name, size_t len) {
  for (int i = 0; i < nsp->sym_occ; i++) {
    if (strncmp(name, nsp->symbols[i].val, len) == 0) {
      printf("%s ,%s\n", name, nsp->symbols[i].val);
      return true;
    }
  }
  return false;
}

void declare_var(Ns *nsp, size_t type, char *name, size_t len) {
  if (nsp->sym_occ >= nsp->sym_len) {
    if ((nsp->symbols =
      realloc(nsp->symbols, (nsp->sym_len += NEY_REALLOC_STEP))) == null)
      err(EXIT_FAILURE, "Heap overflow!");
  }
  nsp->symbols[nsp->sym_occ] = (Symbol){
    .val = malloc(len),
    .type = (Type){.variant = type},
    .len = len,
  };
  strncpy(nsp->symbols[nsp->sym_occ].val, name, len);
  nsp->sym_occ++;
}

bool is_tok(Parser *p, TokenKind kind) {
  return p->lexem->tokens[p->cursor].type == kind;
}

bool is_tok_at(Parser *p, size_t index, TokenKind kind) {
  return p->lexem->tokens[index].type == kind;
}

bool is_type(char *str, size_t len) {
  bool found = false;
  for (int i = 0; i < TYPES_LEN; i++) {
    if (strncmp(str, str_types[i], len) == 0) {
      found = true;
      break;
    }
  }
  return found;
}

bool is_add_or_sub_tok(Parser *p) {
  return p->lexem->tokens[p->cursor].value[0] == '+' ||
  p->lexem->tokens[p->cursor].value[0] == '-';
}

bool is_mul_or_div_tok(Parser *p) {
  return p->lexem->tokens[p->cursor].value[0] == '*' ||
  p->lexem->tokens[p->cursor].value[0] == '/';
}

void get_word_at(Parser *p, size_t i, char *buff, size_t len) {
  if (buff == null) {
    err(EXIT_FAILURE, "cannot put word onto a null buffer !");
  }
  if (len < p->lexem->tokens[i].value_len) {
    err(EXIT_FAILURE,
        "buffer size cannot be smaller than the token word length!");
  }
  snprintf(buff, len, "%.*s", p->lexem->tokens[i].value_len,
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

Ns *ns_new(Ns nsp) {
  static struct {
    uint16_t occ;
    Ns at[UINT16_MAX];
  } data = {0};
  memcpy(&data.at[data.occ], &nsp, sizeof(Ns));
  return &data.at[data.occ++];
}

Parser *parser_new(Parser p) {
  static struct {
    uint16_t occ;
    Parser at[UINT16_MAX];
  } data = {0};
  memcpy(&data.at[data.occ], &p, sizeof(Parser));
  data.at[data.occ].cursor = 0;
  data.at[data.occ].node = 0;
  data.at[data.occ].line = 0;
  data.at[data.occ].bol = 0;
  return &data.at[data.occ++];
}

Ast *ast_new(Ast a) {
  static struct {
    uint16_t occ;
    Ast at[UINT16_MAX];
  } data = {0};
  if (a.variant >= var_len)
    err(EXIT_FAILURE, "ast constructor error: unknown ast kind!\n");
  memcpy(&data.at[data.occ], &a, sizeof(Ast));
  return &data.at[data.occ++];
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

void parser_parse(Parser *p) {
  while(!is_tok_at(p, p->cursor, TOKEN_EOF)) {
    char* curr = p->lexem->tokens[p->cursor].value;
    size_t curr_len = p->lexem->tokens[p->cursor].value_len;
    if (is_tok_at(p, p->cursor, TOKEN_EQUAL)) {
      parser_parse_assign_decl(p);
      if (!is_tok_at(p, p->cursor, TOKEN_EOL))
        err(EXIT_FAILURE, "statement needs to end with a semi-colon !");
    }
    if (p->cursor < p->lexem->len - 2 &&
      p->cursor > 0 &&
      is_tok(p, TOKEN_SYMBOL) &&
      is_tok_at(p, p->cursor + 1, TOKEN_SYMBOL) &&
      is_tok_at(p, p->cursor + 2, TOKEN_EOL))
    {
      printf("decl");
      parser_parse_decl(p);
    }
    if (is_tok(p, TOKEN_SYMBOL) && is_word(curr, "struct", curr_len)) {
    }
    parser_eat(p, p->lexem->tokens[p->cursor].type);
  }
}

Ast* parser_parse_factor(Parser *p) {
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

Ast* parser_parse_term(Parser *p) {
  Ast *curr = parser_parse_factor(p);
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

Ast* parser_parse_expr(Parser *p) {
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

Ast* parser_parse_assign_decl(Parser* p) {
  Ast *node = null;
  if (p->cursor >= 2 && is_tok_at(p, p->cursor - 2, TOKEN_SYMBOL)) {
    size_t len = p->lexem->tokens[p->cursor - 2].value_len + 1; // >>> vartype varname = value
    char type[len];
    get_word_at(p, p->cursor - 2, type, len);
    if (!is_type(type, len))
      err(EXIT_FAILURE, "declaration needs to have a proper type ! ");
    size_t var_len = p->lexem->tokens[p->cursor - 1].value_len + 1; // add 1 for null terminator
    char var_name[var_len];
    get_word_at(p, p->cursor - 1, var_name, var_len);
    if (var_declared(p->nsp, var_name,var_len))
      err(EXIT_FAILURE, "unimplemented variable redeclaration!");
    else {
      size_t type_variant = get_type_from_str(type, len);
      if (type < 0)
        err(EXIT_FAILURE, "declaration needs to have a proper type ! ");
      declare_var(p->nsp, type_variant, var_name, var_len);
      parser_eat(p, p->lexem->tokens[p->cursor].type);
      Ast* node = ast_new((Ast){
        .variant = decl,
        .val = ast_new((Ast){
          .tok = p->lexem->tokens[p->cursor],
          .type = (Type) {.variant = type_variant},
        })
      });
      parser_eat(p, p->lexem->tokens[p->cursor].type);
    }
  }
  return node;
}

Ast* parser_parse_decl(Parser *p) {
  if (p->cursor < 0) return null;
  if (p->cursor >= p->lexem->len) return null;
  if (p->cursor > 0 && is_tok(p, TOKEN_SYMBOL) && is_tok_at(p, p->cursor + 1, TOKEN_SYMBOL)) {
    if (p->cursor >= p->lexem->len - 2)
      err(EXIT_FAILURE, "Syntax error !");
    if (!is_tok_at(p, p->cursor + 2, TOKEN_EOL))
      err(EXIT_FAILURE, "Syntax error !");
    size_t len = p->lexem->tokens[p->cursor].value_len + 1; // vartype varname;
    char type[len];
    get_word_at(p, p->cursor, type, len);
    if (!is_type(type, len))
      err(EXIT_FAILURE, "Syntax error type is not a valid type !");
    size_t var_len = p->lexem->tokens[p->cursor + 1].value_len + 1; // add 1 for null terminator
    char var_name[var_len];
    get_word_at(p, p->cursor + 1, var_name, var_len);
    if (var_declared(p->nsp, var_name, var_len))
      err(EXIT_FAILURE, "unimplemented variable redeclaration");
    size_t t = get_type_from_str(type, len);
    declare_var(p->nsp, t, var_name, var_len);
    for (int i = 0; i < 2; i++) parser_eat(p, p->lexem->tokens[p->cursor].type);
  }
  return null;
}

Ast* parser_parse_struct(Parser *p) {
  if (p->cursor <= 0) return null;
  if (p->cursor >= p->lexem->len) return null;
  if (p->cursor > 0 && is_tok(p, TOKEN_SYMBOL) && is_word(p->lexem->tokens[p->cursor].value, "struct", p->lexem->tokens[p->cursor].value_len)) {
    parser_eat(p, TOKEN_SYMBOL);
    if (!is_tok(p, TOKEN_SYMBOL))
      err(EXIT_FAILURE, "syntax error struct requires a definition identifier");
    parser_eat(p, TOKEN_SYMBOL);
    if (!is_tok(p, TOKEN_CURLY_OPEN))
      err(EXIT_FAILURE, "syntax error struct requires a block");
    parser_eat(p, TOKEN_CURLY_OPEN);
    Type args[256] = {};
    ast *node =  ast_new((Ast){
      .variant = decl,
      .case_struct = {.args = args}
    });
    while (!is_tok(p, TOKEN_CURLY_CLOSE)) {
      Ast* node = parser_parse_decl(p);
      args[i] = (Type) {.variant = node->variant};
      if (!node)
        err(EXIT_FAILURE, "struct member syntax error");
    }
    return node;
  }
  return null;
}

#undef TYPES_LEN
#undef TYPES_PAIR_LEN
