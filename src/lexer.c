#include "../libs/lexer.h"
#include <stdlib.h>

/*  TODO: ADD ERROR SYSTEM */
typedef struct {
  TokenKind kind;
  const char *text;
} Literal_Token;

typedef struct Lexem {
  Token *tokens;
  size_t len;
} Lexem;

Literal_Token literal_tokens[] = {
    {.text = "(", .kind = TOKEN_PARENTHESIS_OPEN},
    {.text = ")", .kind = TOKEN_PARENTHESIS_CLOSE},
    {.text = "{", .kind = TOKEN_CURLY_OPEN},
    {.text = "}", .kind = TOKEN_CURLY_CLOSE},
    {.text = ";", .kind = TOKEN_EOL},
};

const char *keywords[] = {
    "type",    "immut",    "const",  "string", "char",   "float16", "float32",
    "float64", "float128", "uint8",  "uint16", "uint32", "uint64",  "uint128",
    "int8",    "int16",    "int32",  "int64",  "int128", "void",    "enum",
    "break",   "continue", "while",  "for",    "repeat", "until",   "import",
    "from",    "as",       "xor",    "xnor",   "nand",   "nor",     "or",
    "and",     "sizeof",   "typeof", "if",     "else",   "elseif",  "func",
    "default", "case",     "extern", "private"};

#define literal_tokens_count                                                   \
  (sizeof(literal_tokens) / sizeof(literal_tokens[0]))
#define keywords_count (sizeof(keywords) / sizeof(keywords[0]))

char *get_ast_node_name(Ast *node) {
  switch (node->variant) {
  case lit:
    return "AST_NODE LITERAL";
  case expr:
    return "AST_NODE EXPRESSION";
  default:
    return "TOKEN NOT FOUND";
  }
}

char *get_token_name(TokenKind token_type) {
  switch (token_type) {
  case TOKEN_EOL:
    return "END OF LINE";
  case TOKEN_EOF:
    return "TOKEN_E0F";
  case TOKEN_STRING:
    return "TOKEN STRING";
  case TOKEN_NUMBER:
    return "TOKEN NUMBER";
  case TOKEN_OP:
    return "TOKEN OPERATOR";
  case TOKEN_GREATER:
    return "TOKEN_GREATER";
  case TOKEN_SMALLER:
    return "TOKEN_SMALLER";
  case TOKEN_EQUAL:
    return "TOKEN_EQUAL";
  case TOKEN_ISEQ:
    return "TOKEN ISEQUAL";
  case TOKEN_CURLY_OPEN:
    return "TOKEN_OPEN";
  case TOKEN_CURLY_CLOSE:
    return "TOKEN_CLOSE";
  case TOKEN_SYMBOL:
    return "TOKEN_SYMBOL";
  case TOKEN_INVALID:
    return "TOKEN_INVALID";
  default:
    return "TOKEN NOT FOUND";
  }
}

bool is_symbol_start(char x) { return isalpha(x) || x == '_'; }
bool is_symbol(char x) { return isalnum(x) || x == '_'; }
void lexer_trim_left(Lexer *l) {
  while (l->cursor < l->content_len && isspace(l->content[l->cursor])) {
    if (l->cursor > l->content_len)
      NEY_ERR("ERROR DURING COMPILATION TIME");
    l->cursor++;
  }
}

/*

  typedef struct Type {
   enum { t_arr, t_fn, t_int, t_float, t_string, ..etc } variant;
   union {
     struct {
       struct Type *elem;
     } case_arr;
     struct {
       struct Type **args;
       struct Type *ret;
       size_t nArgs;
     } case_fn;
   }
 } Type;

 typedef struct Ast {
   enum { lit, binop, unop, expr, ident, string, number, if_st, loop }
   variant; Token tok; Type type; size_t klen; struct Ast kids[];
 } Ast;

 typedef struct Lexem lexem */

Ast parse_at(Ast *as_nodes, Lexem *lexem, size_t i) {
  if (lexem->tokens[i].type == TOKEN_NUMBER) {
    return (Ast){
        .variant = lit,
        .tok = lexem->tokens[i],
        .type = {},
    };
  }
  char op = lexem->tokens[i].value[0];
  if (lexem->tokens[i].type == TOKEN_OP) {
    if (i <= 0) {
      printf("syntax error on arithmetic operation!");
      exit(EXIT_FAILURE);
    }
    if (lexem->tokens[i - 1].type == TOKEN_NUMBER) {
      as_nodes[i + 1] = parse_at(as_nodes, lexem, i + 1);
      Ast node = (Ast){
          .variant = expr,
          .tok = lexem->tokens[i],
          .type = (Type){},
          .kid1 = &as_nodes[i - 1],
          .kid2 = &as_nodes[i + 1],
      };
      return node;
    }
    return (Ast){};
  }
  return (Ast){};
}

Lexer lexer_init(const char *content, uint32_t content_len) {
  Lexer l = {0};
  l.content = content;
  l.content_len = content_len;
  return l;
}

Token lexer_next(Lexer *l) {
  lexer_trim_left(l);
  Token tok = (Token){.value = &l->content[l->cursor], .type = TOKEN_EOF};

  if (l->cursor > l->content_len)
    return tok;

  if (l->content[l->cursor] == '"') {
    tok.type = TOKEN_STRING;
    tok.value_len += 1;
    l->cursor += 1;
    while (l->cursor < l->content_len && l->content[l->cursor] != '"') {
      tok.value_len += 1;
      l->cursor += 1;
    }
    tok.value_len += 1;
    l->cursor += 1;
    return tok;
  }

  if (l->content[l->cursor] == ';') {
    tok.type = TOKEN_EOL;
    tok.value_len = 1;
    l->cursor += 1;
    return tok;
  }
  if (l->content[l->cursor] == '+' || l->content[l->cursor] == '-' ||
      l->content[l->cursor] == '*' || l->content[l->cursor] == '/') {
    tok.type = TOKEN_OP;
    tok.value_len = 1;
    l->cursor += 1;
    return tok;
  }
  if (l->content[l->cursor] == '=') {
    tok.value_len++;
    if (l->cursor + 1 < l->content_len && l->content[l->cursor + 1] == '=') {
      tok.type = TOKEN_ISEQ;
      tok.value_len += 1;
      l->cursor += 1;
    } else {
      tok.type = TOKEN_EQUAL;
    }
    l->cursor += 1;
    return tok;
  }
  if (isdigit(l->content[l->cursor])) {
    tok.type = TOKEN_NUMBER;
    if (l->content[l->cursor] == '0' && l->cursor + 1 < l->content_len &&
        l->content[l->cursor + 1] == 'x') {
      if (l->cursor + 1 > l->content_len) {
        tok.type = TOKEN_INVALID;
        tok.value_len += 1;
        l->cursor += 1;
        return tok;
      }
      if (!(is_symbol(l->content[l->cursor + 1]) ||
            isdigit(l->content[l->cursor + 1]))) {
        tok.type = TOKEN_INVALID;
        tok.value_len += 1;
        l->cursor += 1;
        return tok;
      }
      while (l->cursor < l->content_len && is_symbol(l->content[l->cursor])) {
        l->cursor += 1;
        tok.value_len += 1;
      }
      return tok;
    }
    while (l->cursor < l->content_len && isdigit(l->content[l->cursor])) {
      l->cursor += 1;
      tok.value_len += 1;
    }
    return tok;
  }
  if (is_symbol_start(l->content[l->cursor])) {
    tok.type = TOKEN_SYMBOL;
    while (l->cursor < l->content_len && is_symbol(l->content[l->cursor])) {
      l->cursor += 1;
      tok.value_len += 1;
    }
    return tok;
  }
  return tok;
}
