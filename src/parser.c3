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
  "let", "func", "as", "end",
  "then", "for", "foreach", "while",
  "len", "typeof",
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

fn bool Parser.is_eof(&self) @inline {
  return (self.tokens.len() == 0 || self.token().kind == TKind.EOF || self.i >= self.tokens.len());
}

fn void Parser.inc(&self) @inline {
  if (self.i >= self.tokens.len()) return;
  self.i += 1;
}

fn void Parser.parse(&self) {
  while (!self.is_eof()) {
    self.token().print();
    io::print("\n");
    self.inc();
  }
}
