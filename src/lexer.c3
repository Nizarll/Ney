import std::io;
import std::collections;
import libc;
import types;

// c-imports

extern fn bool is_digit(char c) @extern("isdigit");
extern fn bool is_alpha(char c) @extern("isalpha");

enum TKind : uint {
  EQ,
  EOF,
  DOT,
  PIPE,
  ISEQ,
  LESS,
  COLON,
  ARROW,
  LESSEQ,
  SYMBOL,
  SEMICOL,
  GREATER,
  GREATEREQ,
  OPENPAREN,
  CLOSEPAREN,
  CLOSEBRACK,
  OPENBRACK,
}

struct Token {
  Slice(<char>) view;
  TKind kind;
}

fn void Token.print(&self) {
  String s;
  switch (self.kind) {
    case EOF:
      s = "EOF";
    case DOT:
      s = "DOT";
    case PIPE:
      s = "PIPE";
    case ISEQ:
      s = "ISEQ";
    case EQ:
      s = "EQ";
    case ARROW:
      s = "ARROW";
    case LESSEQ:
      s = "LESSEQ";
    case LESS:
      s = "LESS";
    case GREATEREQ:
      s = "GREATEREQ";
    case GREATER:
      s = "GREATER";
    case COLON:
      s = "COLON";
    case SEMICOL:
      s = "SEMICOL";
    case OPENPAREN:
      s = "OPENPAREN";
    case CLOSEPAREN:
      s = "CLOSEPAREN";
    case CLOSEBRACK:
      s = "CLOSEBRACK";
    case OPENBRACK:
      s = "OPENBRACK";
  }
  io::print(s);
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
  char curr = self.curr();
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
  self.tokens.push(Token{view, kind});
  for (int i = 0; i < len; i++) self.inc();
}

fn List(<Token>) Lexer.lex(&self) {
  while (self.i < self.view.len) {
    char curr = self.curr();
    switch (curr) {
      case '\t': case '\v':
      case '\r': case '\n':
      case ' ':
        self.inc();
      case ',': self.push_type(TKind.COLON);
      case ';': self.push_type(TKind.SEMICOL);
      case '(': self.push_type(TKind.OPENPAREN);
      case ')': self.push_type(TKind.CLOSEPAREN);
      case ']': self.push_type(TKind.CLOSEBRACK);
      case '[': self.push_type(TKind.OPENBRACK);
      case '|': self.push_type(TKind.PIPE);
      case '.': self.push_type(TKind.DOT);
      case '=': switch(self.peek()) {
        case '=': self.push_type(TKind.ISEQ, 2);
        case '>': self.push_type(TKind.ARROW, 2);
        default: self.push_type(TKind.EQ);
      }
      case '<': switch(self.peek()) {
        case '=': self.push_type(TKind.LESSEQ, 2);
        default: self.push_type(TKind.LESS);
      }
      case '>': switch(self.peek()) {
        case '=': self.push_type(TKind.GREATEREQ, 2);
        default: self.push_type(TKind.GREATER);
      }
      case '\'': self.parse_char();
      case  '"': self.parse_str();
      case is_digit(curr): self.parse_num();
      case is_alpha(curr): self.parse_sym();
    }
  }
  return self.tokens;
}

fn void Lexer.destroy(&self) {
  self.unimplemented();
}

fn void Lexer.unimplemented(&self) {
  io::print("error: [lexer] unimplemented");
  libc::exit(1);
}
