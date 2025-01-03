import std::io;
import libc;
import utils;
import std::collections;

// c-imports

enum TKind : uint (String tostr) {
  EOF = "EOF", PLUS = "PLUS", PIPE = "PIPE", ISEQ = "ISEQ",
  NOTEQ = "NOTEQ", EQ = "EQ", MINUS = "MINUS", DIV = "DIV",
  DOT = "DOT", NOT = "NOT", ARROW = "ARROW", LESSEQ = "LESSEQ",
  SYMBOL = "SYMBOL", NUMLIT = "NUMLIT", SEMICOL = "SEMICOL",
  GREATER = "GREATER", OPENBRACK = "OPENBRACK", MUL = "MUL",
  COMA = "COMA", LESS = "LESS", CLOSECURLY = "CLOSECURLY",
  CHAR = "CHAR", STR = "STR", SHIFTLEFT = "SHIFTLEFT",
  SHIFTRIGHT = "SHIFHTRIGHT", GREATEREQ = "GREATEREQ",
  OPENPAREN = "OPENPAREN", CLOSEPAREN = "CLOSEPAREN",
  CLOSEBRACK = "CLOSEBRACK", OPENCURLY = "OPENCURLY"
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

extern fn CInt is_digit(char c) @extern("isdigit");
extern fn CInt is_alpha(char c) @extern("isalpha");
extern fn CInt is_space(char c) @extern("isspace");
extern fn CInt is_alnum(char c) @extern("isalnum");

fn bool Token.is(&self, TKind kind) @inline {
  return self.kind == kind;
}

fn String Token.tostr(&self) @inline {
  return self.kind.tostr;
}

macro void Token.print(&self) {
  libc::printf("%.*s at: content:%.*s line: %d, func: %s\n", 
  self.tostr().len, self.tostr(),
  self.view.len, self.view.ptr,
  $$LINE, $$FUNC);
}

fn bool ZString.match(&self, String s) @inline {
  return (bool) (libc::strncmp(*self, s, s.len) == 0);
}

fn Lexer init(Slice(<char>) view) {
  Lexer l;
  l.view = view; // other stuff
  return l;
}

fn void Lexer.parse_char(&self) {
  self.push_type(TKind.CHAR); // TODO: fix this
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
    libc::exit(1);
    return;
  }
  usz end = self.i;
  self.push_type(TKind.STR, end - start);
}

fn void Lexer.parse_num(&self) {
  //TODO: handle binary sequences
  //TODO: handle octal and hexadecimal sequences ??
  usz start = self.i;
  while (is_digit(self.curr())) {
    self.inc();
  }
  if (self.i > self.view.len) {
    io::print("error in lexing number literal\n");
    libc::exit(1);
    return;
  }
  usz end = self.i;
  self.i = start;
  self.push_type(TKind.NUMLIT, end - start, false);
  self.inc();
}

fn void Lexer.parse_sym(&self) {
  usz start = self.i;
  while (is_alnum(self.curr()) || self.curr() == '_') {
    self.inc();
  }
  if (self.i > self.view.len) {
    io::print("error in symbol literal\n");
    libc::exit(1);
    return;
  }
  usz end = self.i;
  self.i = start;
  self.push_type(TKind.SYMBOL, end - start);
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

fn char Lexer.peek(&self, int offset = 1) @inline {
  if (self.i + offset >= self.view.len) return '\0';
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

fn void Lexer.push_type(&self, TKind kind, usz len = 1, bool inc = true) {
  Slice(<char>) view = {self.at(self.i), len};
  self.tokens.push(Token{kind, self.loc, view});
  if (!inc) return;
  for (int i = 0; i < len; i++) self.inc();
}

fn void Lexer.lex(&self) {
  while (self.i < self.view.len) {
    char curr = self.curr();
    switch (curr) {
    case '\t': case '\v':
    case '\r': case '\n':
    case ' ':
    self.inc();
    case '+': self.push_type(TKind.PLUS);
    case '/': self.push_type(TKind.DIV);
    case '*': self.push_type(TKind.MUL);
    case '-': self.push_type(TKind.MINUS);
    case '\0': self.push_type(TKind.EOF);
    case ',': self.push_type(TKind.COMA);
    case ';': self.push_type(TKind.SEMICOL);
    case '{': self.push_type(TKind.OPENCURLY);
    case '}': self.push_type(TKind.CLOSECURLY);
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
      case '<': self.push_type(TKind.SHIFTLEFT, 2);
      case '=': self.push_type(TKind.LESSEQ, 2);
      default: self.push_type(TKind.LESS);
    }
    case '!': switch(self.peek()) {
      case '=': self.push_type(TKind.NOTEQ, 2);
      default: self.push_type(TKind.NOT);
    }
    case '>': switch(self.peek()) {
      case '>': self.push_type(TKind.SHIFTRIGHT, 2);
      case '=': self.push_type(TKind.GREATEREQ, 2);
      default: self.push_type(TKind.GREATER);
    }
    case  '"': self.parse_str();
    case '\'': self.parse_char();
    default:
      if (is_digit(curr)) { self.parse_num(); }
      else if (curr == '_' || is_alnum(curr)) {
        self.parse_sym();
      }
      else {
        //TODO: ADD ERROR HERE 
        io::print("\n");
        io::printf("%c", curr);
        io::print("\n");
        io::print("unexpetcetd token");
        libc::exit(1);
      }
    }
  }
}

fn void Lexer.destroy(&self) {
 lx_unimplemented();
}

macro void lx_unimplemented() {
  libc::printf("[Lexer]: Unimplemented At: %s Line %d\n", $$FUNC, $$LINE);
  libc::exit(1);
}
