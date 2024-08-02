# 1 "libs/parser.h"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 390 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "libs/parser.h" 2
# 20 "libs/parser.h"
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
# 42 "libs/parser.h"
enum class AstVariant : u8 {
  LIT,
  EXPR,
  DECL,
  FN,
  STRUCT,
  BINOP,
  BITOP,
  LEN,
};

enum class PrimaryType : u8 {
  i8, i16, i32, i64, i128, u8, u16, u32, u64, u128, f32, f64, f128, string, chr,
  LEN
};

enum class TokenKind : u8 {
  END_OF_FILE, RPAREN, LPAREN, RBRACK, LRBACK, RCURLY, LCURLY, IDENT, CHAR, STR, SYMBOL, EOL, SEMICOL, COL, UNDEFINED,
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
  err() = default;
  template <typename T> requires std::same_as<std::remove_cvref_t<T>, std::span<char>>
  err(ErrorKind m_kind, T&& m_loc,const char* m_msg) : kind{std::to_underlying(m_kind)}, loc(std::forward<T>(m_loc)), msg(m_msg)
  {}
  auto to_str() const;
};

struct token {
  u16 kind;
  std::span<char> loc;
  token() = default;
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
  void consume_if(u16 type);
  void consume(u16 amnt);
  void consume_spaces();
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
  std::variant<case_struct, case_arr> c;
};

struct ast {
  type tp;
  AstVariant variant;
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
  bool is_string(const char* lit);
  bool is_tok(TokenKind kind);
  void consume(u16 amnt);
  void consume_if(TokenKind kind);
  token prev_tok();
  token next_tok();
  ast* parse_decl();
  ast* parse_struct();
  ast_list parse_lexem(lexem& l);
};

bool is_symbol(char c);
