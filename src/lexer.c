#include "../libs/lexer.h"

typedef struct {
  Token_Type kind;
  const char *text;
} Literal_Token;

Literal_Token literal_tokens[] = {
    {.text = "(", .kind = TOKEN_PARENTHESIS_OPEN},
    {.text = ")", .kind = TOKEN_PARENTHESIS_CLOSE},
    {.text = "{", .kind = TOKEN_CURLY_OPEN},
    {.text = "}", .kind = TOKEN_CURLY_CLOSE},
    {.text = ";", .kind = TOKEN_SEMICOLON},
};

const char *keywords[] = {
    "immut",    "const", "string", "char",   "float16", "float32", "float64",
    "float128", "uint8", "uint16", "uint32", "uint64",  "uint128", "int8",
    "int16",    "int32", "int64",  "int128", "void",    "enum",    "break",
    "continue", "while", "for",    "repeat", "until",   "import",  "from",
    "as",       "xor",   "xnor",   "nand",   "nor",     "sizeof",  "typeof",
    "if",       "else",  "elseif", "func",   "default", "case",    "extern",
    "private"};

#define literal_tokens_count                                                   \
  (sizeof(literal_tokens) / sizeof(literal_tokens[0]))
#define keywords_count (sizeof(keywords) / sizeof(keywords[0]))

const char *get_token_name(Token_Type token_type) {
  switch (token_type) {
  case TOKEN_EOL:
    return "END OF LINE";
  case TOKEN_EOF:
    return "TOKEN_E0F";
  case TOKEN_STRING:
    return "TOKEN STRING";
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
    tok.value_len++;
    l->cursor++;
    while (l->cursor < l->content_len && l->content[l->cursor] != '"') {
      tok.value_len++;
      l->cursor++;
    }
    tok.value_len++;
    l->cursor++;
    return tok;
  }

  if (l->content[l->cursor] == ';') {
    tok.type = TOKEN_EOL;
    tok.value_len = 1;
    l->cursor++;
    return tok;
  }

  if (l->content[l->cursor] == '=') {
    tok.value_len++;
    if (l->content[l->cursor + 1] == '=') {
      NEY_WARN("this is a double eq");
      tok.type = TOKEN_ISEQ;
      tok.value_len++;
      l->cursor++;
    } else {
      tok.type = TOKEN_EQUAL;
    }
    l->cursor++;
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
