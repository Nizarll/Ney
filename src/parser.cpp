#include "../libs/parser.h"
#include <utility>

#define ANSI_COLOR_RESET             "\x1b[0m"
#define ANSI_COLOR_BLACK             "\x1b[30m"
#define ANSI_COLOR_RED               "\x1b[31m"
#define ANSI_COLOR_GREEN             "\x1b[32m"
#define ANSI_COLOR_YELLOW            "\x1b[33m"
#define ANSI_COLOR_BLUE              "\x1b[34m"
#define ANSI_COLOR_MAGENTA           "\x1b[35m"
#define ANSI_COLOR_CYAN              "\x1b[36m"
#define ANSI_COLOR_WHITE             "\x1b[37m"
#define ANSI_COLOR_BRIGHT_BLACK      "\x1b[30;1m"
#define ANSI_COLOR_BRIGHT_RED        "\x1b[31;1m"
#define ANSI_COLOR_BRIGHT_GREEN      "\x1b[32;1m"
#define ANSI_COLOR_BRIGHT_YELLOW     "\x1b[33;1m"
#define ANSI_COLOR_BRIGHT_BLUE       "\x1b[34;1m"
#define ANSI_COLOR_BRIGHT_MAGENTA    "\x1b[35;1m"
#define ANSI_COLOR_BRIGHT_CYAN       "\x1b[36;1m"
#define ANSI_COLOR_BRIGHT_WHITE      "\x1b[37;1m"
#define ANSI_BG_COLOR_BLACK          "\x1b[40m"
#define ANSI_BG_COLOR_RED            "\x1b[41m"
#define ANSI_BG_COLOR_GREEN          "\x1b[42m"
#define ANSI_BG_COLOR_YELLOW         "\x1b[43m"
#define ANSI_BG_COLOR_BLUE           "\x1b[44m"
#define ANSI_BG_COLOR_MAGENTA        "\x1b[45m"
#define ANSI_BG_COLOR_CYAN           "\x1b[46m"
#define ANSI_BG_COLOR_WHITE          "\x1b[47m"
#define ANSI_BG_COLOR_BRIGHT_BLACK   "\x1b[40;1m"
#define ANSI_BG_COLOR_BRIGHT_RED     "\x1b[41;1m"
#define ANSI_BG_COLOR_BRIGHT_GREEN   "\x1b[42;1m"
#define ANSI_BG_COLOR_BRIGHT_YELLOW  "\x1b[43;1m"
#define ANSI_BG_COLOR_BRIGHT_BLUE    "\x1b[44;1m"
#define ANSI_BG_COLOR_BRIGHT_MAGENTA "\x1b[45;1m"
#define ANSI_BG_COLOR_BRIGHT_CYAN    "\x1b[46;1m"
#define ANSI_BG_COLOR_BRIGHT_WHITE   "\x1b[47;1m"
#define ANSI_STYLE_BOLD              "\x1b[1m"
#define ANSI_STYLE_UNDERLINE         "\x1b[4m"
#define ANSI_STYLE_REVERSED          "\x1b[7m"

constexpr auto TOKEN_UNKNOWN_ERR = "Tokenization error"
  ": unknown token !";
constexpr auto TOKEN_JUMP_ERR = "Cannot jump by specified amount!";
constexpr auto TOKEN_INVALID_ERR = "Cannot consume token that is different to the expected token!";

std::array KindString = {
  TOKENS(STRING)
};

std::array TypeString = {
  TYPES(STRING)
};

#undef NOP
#undef TYPES
#undef TOKENS
#undef STRING

std::array ErrorString = {
  ANSI_COLOR_YELLOW "[warning]" ANSI_COLOR_RESET,
  ANSI_COLOR_BRIGHT_RED "[error]" ANSI_COLOR_RESET,
  ANSI_COLOR_BRIGHT_BLUE "[note]" ANSI_COLOR_RESET,
};

auto err::to_str() const {
  assert(kind <= ErrorString.size());
  return ErrorString[kind];
};

auto token::to_str() const {
  assert(kind <= KindString.size());
  return KindString[kind];
}

template <typename T> requires std::is_same_v<std::remove_cvref_t<T>, err>
void throw_err(T&& error) {
  std::printf("%s: %s\n\tat: %s\n\t~~~~", error.to_str(), error.msg.data(), error.loc.data());
  for (auto _ : error.loc)
  putchar('~');
  putchar('\n');
  if (error.kind == std::to_underlying(ErrorKind::ERROR)) exit(EXIT_FAILURE);
}

/************[[lexing]]***************/

std::unordered_map<char, TokenKind> TokenData = {
  {')', TokenKind::RPAREN},
  {'(', TokenKind::LPAREN},
  {']', TokenKind::RBRACK},
  {'[', TokenKind::LRBACK},
  {'{', TokenKind::RCURLY},
  {'}', TokenKind::LCURLY},
  {'\'', TokenKind::CHAR},
  {'\"', TokenKind::STR},
  {'\n', TokenKind::EOL},
  {':', TokenKind::COL},
  {';', TokenKind::SEMICOL},
};

void lexer::consume_if(u16 type) {
  assert(type < std::to_underlying(TokenKind::LEN));
  throw_err(err(
    ErrorKind::ERROR,
    std::span<char>(content + cursor, 1),
    "UNIMPLEMENTED"
  ));
}

void lexer::consume(u16 amnt) {
  if (cursor + amnt >= len) {
    throw_err(err(
      ErrorKind::ERROR,
      std::span<char>(content + cursor, amnt),
      TOKEN_JUMP_ERR
    ));
  }
  cursor += amnt;
}

void lexer::consume_spaces() {
  while (isspace(content[cursor])) {
    if (cursor >= len)
    throw_err(err(
        ErrorKind::ERROR,
        std::span<char>(content + cursor, 1),
        TOKEN_JUMP_ERR
      ));
    consume(1);
  }
}

token lexer::next() {

  consume_spaces();
  token t(TokenKind::END_OF_FILE, std::span<char>(content, 1));
  if (cursor >= len) return t;
  if (is_symbol(content[cursor])) {
    t.set_kind(TokenKind::SYMBOL);
    size_t t_len = 1, prev_cursor = cursor;
    while (cursor < len && is_symbol(content[cursor])) {
      t_len += 1;
      cursor += 1;
    }
    t.loc = std::span<char>(content + prev_cursor, t_len);
    return t;
  }
  t.loc = std::span<char>(content + cursor, 1);
  auto it = TokenData.find(content[cursor]);
  if (it == TokenData.end()) {
    t.set_kind(TokenKind::UNDEFINED);
    return t;
  }
  t.set_kind(it->second);
  return t;
}

lexem lexer::lex() {
  lexem l;
  token t;
  do {
    t = next();
    std::print("received token : {}, {}\n", t.to_str(), std::string_view(t.loc.data(), t.loc.size()));
    l.emplace_back(t);
    if (cursor < len) consume(1);
  } while (t.kind != std::to_underlying(TokenKind::END_OF_FILE));
  return l;
}

/************[[parsing]]***************/

template <typename T> requires std::is_same_v<std::remove_cvref_t<T>, ast>
void print(T curr_ast) {
  std::print("Ast node Dump : \nkind : {}, {}, {}", KindString[curr_ast.tp.kind]);
}

void parser::consume(u16 amnt) {
  if (curr + amnt >= lex.size())
    throw_err(err(
      ErrorKind::ERROR,
      lex[curr].loc,
      TOKEN_JUMP_ERR
    ));
  curr += amnt;
}

void parser::consume_if(TokenKind kind) {
  assert(curr <= lex.size() && curr >= 0);
  if (lex[curr].kind != std::to_underlying(kind))
    throw_err(err(
      ErrorKind::ERROR,
      lex[curr].loc,
      TOKEN_INVALID_ERR
    ));
  consume(1);
}

ast* parser::parse_decl() {
  ast* node = new ast;
  node->variant = AstVariant::DECL;
  bool resolved = false;
  for (auto& type : TypeString) {
    if (!is_string(type)) continue;
    if (strlen(type) != lex[curr].loc.size());
    u16 val = std::abs(std::distance(KindString.begin(),
                             std::find(KindString.begin(), KindString.end(), type)));
    assert(val <= std::to_underlying(PrimaryType::LEN));
    node->tp.kind = static_cast<PrimaryType>(val);
    resolved = true;
    break;
  }
  assert(curr < lex.size());
  if (!resolved) throw_err(err(
    ErrorKind::ERROR,
    lex[curr].loc,
    "variable declaration holds unknown type"
  ));
  std::print("LIST OF TOKENS: \n");
  for (auto& tok : lex) {
    std::print("tok : {}\n", tok.to_str());
  }
  consume_if(TokenKind::SYMBOL);
  return node;
}

ast* parser::parse_struct() {
  assert(curr <= lex.size() && curr >= 0);
  auto node = new ast;
  node->variant = AstVariant::STRUCT;
  for (int i = 1; i < 2; i++) consume_if(TokenKind::SYMBOL);
  consume_if(TokenKind::LCURLY);
  while (!is_tok(TokenKind::RCURLY)) {
    ast* decl = parse_decl();
  };
  return node;
}

ast_list parser::parse_lexem(lexem &l) {
  lex = l;
  std::vector<ast*> asts;
  parse_decl();
  return ast_list{asts};
}

bool parser::is_string(const char* lit) {
  std::size_t len = std::strlen(lit);
  if (lex[curr].loc.size() != len) return false;
  return std::equal(lex[curr].loc.begin(), lex[curr].loc.end(), lit);
}

bool parser::is_tok(TokenKind kind) {
  assert(curr < lex.size());
  return lex[curr].kind == std::to_underlying(kind);
}

token parser::next_tok() {
  assert(curr - 1 >= 0);
  return lex[curr - 1];
}

token parser::prev_tok() {
  assert(curr + 1 < lex.size());
  return lex[curr + 1];
}

bool is_symbol(char c) { return isalpha(c) || c == '_'; }
