import libc;
import ast;
import std::io;
import types;
import lexer;
import utils;
import std::collections;

fault ParserFaults {
  OUT_OF_BOUNDS,
}

struct Ns {
  void* todo;
}

struct Parser {
  usz i;
  List(<Token>)* tokens;
  List(<Ast*>) nodes;
  List(<Ns>) namespaces;
}

String[*] keywords = {
  "let", "fn", "as", "end", "match",
  "then", "for", "foreach", "while",
  "len", "typeof", "if", "elseif", "else",
  "import", "export", "do",
};

String[*] types = {
  "u64", "u32", "u16", "u8",
  "f32", "f64", "str", "char",
  "bool",
};

fn Parser init(List(<Token>)* tokens) {
  Parser p;
  p.i = 0;
  p.tokens = tokens;
  return p;
}

fn Token Parser.token(&self) @inline {
  return self.tokens.get(self.i);
}

fn Token Parser.peek(&self, int offset = 1) {
  if (self.i + offset < 0) return Token{TKind.EOF, {}, {}};
  if (self.i + offset >= self.tokens.len()) return Token{TKind.EOF, {}, {}};
  return self.tokens.get(self.i + offset);
}

fn bool Parser.is_token(&self, TKind kind) @inline {
  return (self.tokens.len() != 0 && self.i < self.tokens.len() && self.token().kind == kind);
}

fn bool Parser.is_eof(&self) @inline {
  return (self.tokens.len() == 0 || self.token().kind == TKind.EOF || self.i >= self.tokens.len());
}

fn void Parser.inc(&self) @inline {
  if (self.i >= self.tokens.len()) return;
  self.i += 1;
}

fn bool Parser.match(&self, String s) @inline {
  Slice(<char>) view = self.token().view;
  if (view.len == s.len) {
    return libc::memcmp(self.token().view.ptr, (char*) s, s.len) == 0;
  }
  return false;
}

fn void Parser.parse(&self) {
  bool printed = false;
  foreach(&token : self.tokens) {
    token.print();
  }
  self.i = 0;
  while (!self.is_eof()) {
    if (self.is_stmt()) self.parse_stmt();
  }
}

fn bool Parser.is_stmt(&self) {
  if (self.is_token(TKind.SYMBOL) && self.peek().is(TKind.SYMBOL)) return true;
  if (self.is_ns_type()) return true;
  if (self.is_assign()) return true;
  if (self.is_call()) return true;
  return false;
}

fn String* Parser.get_primitive_type(&self) {
  foreach(&type : types) {
    if (self.match(*type)) return type;
  }
  return null;
}

fn bool Parser.is_ns_type(&self) {
  //TODO: 
  //todo do this shit im too lazy rn im ngl
  return false;
}

fn bool Parser.is_assign(&self) {
  TKind[*] kinds = {TKind.SYMBOL, TKind.EQ};
  //TODO: refactor this bullshit im lazy af
  foreach (kind : kinds) {
    if (!self.is_token(kind)) {
      return false;
    }
  }
  return true;
}

fn bool Parser.is_comparison(&self) @inline {
  return (self.is_token(TKind.ISEQ)   || self.is_token(TKind.NOTEQ)  ||
          self.is_token(TKind.LESSEQ) || self.is_token(TKind.GREATEREQ) ||
          self.is_token(TKind.LESS)   || self.is_token(TKind.GREATER));
}

fn bool Parser.is_call(&self) {
  //TODO:
  TKind[*] kinds = {TKind.SYMBOL, TKind.OPENPAREN};
  foreach (kind : kinds) {
    if (!self.is_token(kind)) return false;
  }
  return false;
}

fn bool Parser.is_lit(&self, uint offset = 0) {
  return (self.peek(2).is(TKind.STR)     ||
          self.peek(2).is(TKind.CHAR)    ||
          self.peek(2).is(TKind.CHAR)    ||
          self.peek(2).is(TKind.NUMLIT));
}

fn void Parser.parse_stmt(&self) {
  if (self.is_token(TKind.SYMBOL) && self.peek().is(TKind.SYMBOL)) self.parse_decl();
  //if (self.is_ns_type());
  //if (self.is_assign());
  //if (self.is_call());
}

fn void Parser.parse_ifstmt(&self) {
  _unimplemented();
}

fn void Parser.parse_match_stmt(&self) {
  _unimplemented();
}

fn void Parser.parse_decl(&self) {
  Ast *ast;
  String* primitive_type = self.get_primitive_type();

  Token name_tok = self.peek();
  if (!name_tok.is(TKind.SYMBOL)) {
    _unimplemented("EXPECTED VARIABLE NAME + TODO: add correct error system");
  }
  self.inc();

  if (!primitive_type) {
    _unimplemented("HANDLE USER DEFINED TYPES");
  }

  self.inc();
  if (self.token().is(TKind.EQ) && self.is_lit()) {
      ast = ast::create_decl(
        name_tok.view,
        primitive_type.to_slice(),
        null,
        ast::PRIMITVE_TYPE
      );
      self.inc();
  } else if (self.peek().is(TKind.EQ)) {
    _unimplemented("user defined type after equal");
  }
}

fn Maybe(<Ast*>) parse_expr(&self) {
  Ast* ast;
  while (self.is_comparison()) {
    Ast* left = ;
    Maybe<Ast*> right = self.parse_term();
  }
  return Maybe{ast, ""};
}

fn Maybe(<Ast*>) parse_term(&self) {
  Ast* ast;
  while (self.token.is(TKind.PLUS) || self.token.is(TKind.MINUS)) {
    Ast* left;
    Maybe<Ast*> = self.parse_factor();
  }
  return unexpected("parse_term unexpected");
}

fn Maybe(<Ast*>) parse_binexpr(&self) {
  Ast* ast;
  while (self.is_binop()) {
    Ast* left;
    Maybe<Ast*> right = self.parse_call();
  }
  return unexpected("parse_binexpr unexpected");
}

//fn Maybe()

macro _unimplemented(String s = "") {
  libc::printf("[Parser]: Unimplemented %.*s At: %s Line %d\n", s.len, s, $$FUNC, $$LINE);
  libc::exit(1);
}
