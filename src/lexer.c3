import std::io;
import std::collections;
import libc;
import types;

enum TKind : uint {
  EOF,
  COLON,
  SEMICOL,
  OPENPAREN,
  CLOSEPAREN,
}

struct Token {
  Slice(<char>) view;
  TKind kind;
}

struct Lexer {
  usz i;
  usz row;
  usz col;
  Slice(<char>) view;
  List(<Token>) tokens;
}

fn bool ZString.match(&self, String s) {
  return (bool) (libc::strncmp(*self, s, s.len) == 0);
}

fn Lexer init(Slice(<char>) view) {
  Lexer l;
  l.view = view; // other stuff
  return l;
}

fn void Lexer.inc(&self) {
  switch (self.curr()) {
    case '\n':
      self.col = 1;
      self.row += 1;
    default:
      self.col += 1;
  }
  self.i += 1;
}

fn char Lexer.peek(&self, int offset = 1) {
  if (self.i + offset >= self.view.len) {
    return '\0';
  }
  return self.view[self.i + offset];
}

fn bool Lexer.match(&self, String s) {
  return self.at(self.i).match(s);
}

fn ZString Lexer.at(&self, usz at = 0) @operator([]) {
  return (ZString)&self.view[at];
}

fn char Lexer.curr(&self) {
  return *(char*)self.at(self.i);
}

fn void Lexer.push_type(&self, TKind kind, usz len = 1) {
  Slice(<char>) view = {self.at(self.i), len};
  self.tokens.push({view, kind});
}

fn List(<Token>) Lexer.lex(&self) {
  List(<Token>) list;
  while (self.i < self.view.len) {
    char curr = self.curr();
    switch (curr) {
      case ' ': case '\t': case '\v': case '\r': case '\n':
        self.inc();
      case ',': self.push_type(TKind.COLON);
      case ';': self.push_type(TKind.SEMICOL);
      case '(': self.push_type(TKind.OPENPAREN);
      case ')': self.push_type(TKind.CLOSEPAREN);
      case ']': self.push_type(TKind.CLOSEBRACK);
      case '[': self.push_type(TKind.OPENBRACK);
      case '|': self.push_type(TKind.PIPE);
      case '.': self.push_type(TKind.DOT);
      case '<': self.push_type(TKind.LESS);
      case '>': switch(self.peek()) {
        case '=':
          self.push_type(TKind.ARR, 2);
        default:
          self.push_type(TKind.GREATER);
        }
    }
    self.i++;
  }
  return list;
}

fn void Lexer.destroy(&self) {
  self.unimplemented();
}

fn void Lexer.unimplemented(&self) {
  io::print("error: [lexer] unimplemented");
  libc::exit(1);
}
