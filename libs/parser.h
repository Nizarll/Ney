#pragma once

#include <print>
#include <cctype>
#include <cassert>
#include <concepts>
#include <string_view>
#include <span>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <vector>
#include <expected>
#include <unordered_map>

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

#define STRING($) #$
#define NOP($) $

#define TYPES($) $(i8), $(i16), $(i32), $(i64), $(i128), \
                 $(u8), $(u16), $(u32), $(u64), $(u128),  \
                 $(f32), $(f64), $(f128), $(string),       \
                 $(chr)

#define TOKENS($) $(END_OF_FILE), $(RPAREN), $(LPAREN), $(RBRACK), \
                $(LRBACK), $(RCURLY), $(LCURLY), $(IDENT), $(CHAR), \
                $(STR), $(SYMBOL), $(EOL), $(UNDEFINED)

enum class PrimaryType : u8 {
  TYPES(NOP),
  LEN
};

enum class TokenKind : u8 {
  TOKENS(NOP),
  LEN
};

enum class ErrorKind : u8 {
  WARNING,
  ERROR,
  NOTE,
};

struct err {
  u16 kind;
  std::span<char> loc;
  std::string_view msg;
  template <typename T> requires std::same_as<std::remove_cvref_t<T>, std::span<char>>
  err(ErrorKind m_kind, T&& m_loc,const char* m_msg) : kind{std::to_underlying(m_kind)}, loc(std::forward<T>(m_loc)), msg(m_msg)
  {}
  auto to_str() const;
};

struct token {
  u16 kind;
  std::span<char> loc;
  template <typename T> requires std::same_as<std::remove_cvref_t<T>, std::span<char>>
  token(TokenKind m_kind, T&& m_loc) : kind{std::to_underlying(m_kind)},loc(std::forward<T>(m_loc)) {}
  auto to_str() const;
  void set_kind(TokenKind m_kind) {
    kind = std::to_underlying(m_kind);
  };
};

using lexem = std::vector<token>;

struct lexer {
  char* content;
  u32 cursor;
  u32 line;
  u32 row;
  u32 len;
  std::expected<void, err> consume_if(u16 type);
  std::expected<void, err> consume(u16 amnt);
  std::expected<void, err> consume_spaces();
  lexem lex();
  token next();
};

struct scope {
  scope *prev;
};

struct type {
  PrimaryType kind;
  struct case_struct { type* curr; type* next; };
  struct case_arr { type* elems; };
  std::variant<case_struct, case_arr> var;
};

struct ast {
  type tp;
  struct bi_node{ ast* left; ast* right; };
  struct mono_node{ ast* value; };
  std::variant<bi_node, mono_node> node;
};

struct ast_list {
  std::vector<ast*> nodes;
  void destroy() {
    for (auto& node : nodes)
      delete node;
  }
};

struct parser {
  lexem lex;
  u32 curr;
  ast* parse_struct();
  ast_list parse_lexem(lexem& l);
  void consume(u16 amnt);
};

bool is_symbol(char c);
void throw_err(err&& error);
void throw_err(const err& error);
