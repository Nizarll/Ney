import libc;
import ast;
import std::io;
import utils;
import types;
import lexer;
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
  "let", "func", "as", "end", "match",
  "then", "for", "foreach", "while",
  "len", "typeof", "if", "elseif", "else",
  "import", "export"
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
  return (bool) libc::memcmp(self.peek().view.ptr, (char*) s, s.len);
}

fn void Parser.parse(&self) {
  bool printed = false;
  while (!self.is_eof()) {
    self.peek().print();
    io::print("\n");
    self.inc();
  }
  io::print("\n");
  self.i = 0;
  while (!self.is_eof()) {
    if (self.is_stmt()) self.parse_stmt();
  }
}

fn bool Parser.is_stmt(&self) {
  if (self.get_trivial_type()) return true;
  if (self.is_ns_type()) return true;
  if (self.is_assign()) return true;
  if (self.is_call()) return true;
  return false;
}

fn String* Parser.get_trivial_type(&self) {
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
  foreach (kind : kinds) { //TODO: refactor this bullshit im lazy af
    if (!self.is_token(kind)) return false;
  }
  return true;
}

fn bool Parser.is_call(&self) {
  //TODO:
  TKind[*] kinds = {TKind.SYMBOL, TKind.OPENPAREN};
  foreach (kind : kinds) {
    if (!self.is_token(kind)) return false;
  }
  return false;
}

fn void Parser.parse_stmt(&self) {
  if ((bool)self.get_trivial_type()) self.parse_decl();
  if (self.is_ns_type());
  if (self.is_assign());
  if (self.is_call());
}

fn void Parser.parse_ifstmt(&self) {
  ps_unimplemented();
}

fn void Parser.parse_match_stmt(&self) {
  ps_unimplemented();
}

fn void Parser.parse_decl(&self) {
  Ast *ast = malloc(Ast.sizeof);
  //TODO: add error system;
  ast.kind = AstKind.VARDECL;
  ast.unyon.var_decl.type.name = *self.get_trivial_type();
  //if (!ast.unyon.var_decl.type.name) {
  // ast.type = self.get_ns_type();
  //}
  if (!ast.unyon.var_decl.type.name) {
    ps_unimplemented();
  }
  Token name_tok = self.token();
  self.inc();

  if (self.token(TKind.EQ)) {
    io::print("DECLARATION");
    //TODO: SET VALUE
    self.inc();
    self.inc();
  }

  //TODO: handle non trivial type
  self.nodes.push(ast);
}

fn void Parser.parse_fn(&self) {
  ps_unimplemented();
}

macro void ps_unimplemented() {
  libc::printf("[Parser]: Unimplemented At: %s Line %d\n", $$FUNC, $$LINE);
  libc::exit(1);
}
