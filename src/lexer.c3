import std::io;
import std::collections;
import libc;
import types;

// c-imports

def Location = usz[<2>];

enum TKind : uint {
  PIPE, ISEQ, EQ, EOF,
  DOT, ARROW, LESSEQ,
  SYMBOL, NUMLIT, SEMICOL,
  GREATER, GREATEREQ,
  OPENPAREN, CLOSEPAREN,
  CLOSEBRACK, OPENBRACK,
  COLON, LESS, CHAR, STR,
}

struct Token {
  TKind kind;
  Location loc;
  Slice(<char>) view;
}

struct Lexer {
  usz i;
  Location loc;
  Slice(<char>) view;
  List(<Token>) tokens;
}

extern fn bool is_digit(char c) @extern("isdigit");
extern fn bool is_alpha(char c) @extern("isalpha");


fn void Token.print(&self) {
  String s;
  switch (self.kind) {
    case STR:
      s = "STR";
    case CHAR:
      s = "CHAR";
    case NUMLIT:
      s = "NUMLIT";
    case SYMBOL:
      s = "SYMBOL";
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

fn bool ZString.match(&self, String s) {
  return (bool) (libc::strncmp(*self, s, s.len) == 0);
}

fn Lexer init(Slice(<char>) view) {
  Lexer l;
  l.view = view; // other stuff
  return l;
}

fn void Lexer.parse_char(&self) {
  self.push_type(TKind.CHAR);
}

fn void Lexer.parse_str(&self) {

  char delim = self.curr();
  self.inc();
  usz start = self.i;

  while(self.i < self.view.len && self.curr() != delim) {
    if (self.curr() == '\\') self.inc(); // skip escape sequences;
    self.inc();
  }

//TODO: add error system here
  if (self.i >= self.view.len) {
    io::print("string needs to be closed\n");
    return;
  }

  usz end = self.i;
  self.push_type(TKind.STR, end - start);
}

fn void Lexer.parse_num(&self) {

}

fn void Lexer.parse_sym(&self) {

}

fn void Lexer.inc(&self) {
  char curr = self.curr();
  switch (self.curr()) {
    case '\n':
    self.loc.x = 1;
    self.loc.y += 1;
    default:
    self.loc.x += 1;
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
  self.tokens.push(Token{kind, self.loc, view});
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
      case  '"': self.parse_str();
      case '\'': self.parse_char();
      default:
        switch {
          case is_digit(curr): self.parse_num();
          case is_alpha(curr): self.parse_sym();
        }
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
