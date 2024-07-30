#include "../libs/parser.h"
#include <cstdio>
#include <utility>

constexpr auto TOKEN_UNKNOWN_ERR = "Tokenization error"
  ": unknown token !";
constexpr auto TOKEN_JUMP_ERR = "Cannot jump by specified amount!";

std::array KindString = {
  TOKENS(STRING)
};

#undef STRING
#undef NOP
#undef TYPES

std::array ErrorString = {
  "[warning]",
  "[error]",
  "[note]",
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
  std::print("{}: {}\n\t{}\n\t", error.to_str(), error.msg, error.loc);
  for (auto ch : error.loc)
    putchar('^');
}

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
};

std::expected<void, err> lexer::consume_if(u16 type) {
  assert(type < std::to_underlying(TokenKind::LEN));
  return std::unexpected<err>(err(
    ErrorKind::ERROR,
    std::span<char>(content + cursor, 1),
    "UNIMPLEMENTED"
  ));
}

std::expected<void, err> lexer::consume(u16 amnt) {
  if (cursor + amnt >= len) {
    return std::unexpected<err>(err(
      ErrorKind::ERROR,
      std::span<char>(content + cursor, 1),
      TOKEN_JUMP_ERR
    ));
  }
  cursor += amnt;
  return {};
}

std::expected<void, err> lexer::consume_spaces() {
  while (isspace(content[cursor])) {
    if (cursor >= len)
      return std::unexpected<err>(err(
        ErrorKind::ERROR,
        std::span<char>(content + cursor, 1),
        TOKEN_JUMP_ERR
      ));
    return consume(1);
  }
  return {};
}

token lexer::next() {
  auto _consume = consume_spaces();
  token t(TokenKind::END_OF_FILE, std::span<char>(content, 1));

  if (cursor >= len) return t;
 
  if (!_consume.has_value()) {
    t.set_kind(TokenKind::UNDEFINED);
    return t;
  }

  if (is_symbol(content[cursor])) {
    t.set_kind(TokenKind::SYMBOL);
    size_t t_len = 1;
    while (cursor < len && is_symbol(content[cursor])) {
      t_len += 1;
      cursor += 1;
    }
    t.loc = std::span<char>(content + cursor, t_len);
    return t;
  }
  t.loc = std::span<char>(content + cursor, 1);
  auto it = TokenData.find(content[cursor]);
  if (it == TokenData.end()) {
    t.set_kind(TokenKind::UNDEFINED);
    return t;
  }
  t.set_kind(it->second);
  consume(1);
  return t;
}

lexem lexer::lex() {
  lexem l;
  token t = next();
  l.emplace_back(t);
  while (t.kind != std::to_underlying(TokenKind::END_OF_FILE)) {
    t = next();
    l.emplace_back(t);
  }
  return l;
}

ast* parser::parse_struct() {
  auto node = new ast;

  return node;
}

ast_list parser::parse_lexem(lexem &l) {
  std::vector<ast*> asts;
  return ast_list{asts};
}

bool is_symbol(char c) { return isalpha(c) || c == '_'; }
