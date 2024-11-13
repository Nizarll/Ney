import std::io;
import std::collections;
import libc;
import types;

struct Token {
  Slice(<char>) view;
}

struct Lexer {
  usz row;
  usz col;
  usz i  ;
  Slice(<char>) view;
  List(<Token>) tokens;
}

def Char = char;

fn bool Char.match(Char* self, String s) {
  return (bool) (libc::strncmp(self, s, s.len) == 0);
}

fn Lexer init(Slice(<char>) view) {
  Lexer l;
  l.view = view; // other stuff
  return l;
}
fn Char* Lexer.at(Lexer* self, usz at = 0) @operator([]) {
  return (Char*)&self.view[at];
}

fn Char* Lexer.curr(Lexer* self) {
  return self.at(self.i);
}

fn List(<Token>) Lexer.lex(Lexer* self) {
  List(<Token>) list;
  while (self.i < self.view.len) {
    if (self.curr().match("struct")) {
      io::print("struct");
    } else if (self.curr().match("test")) {
      io::print("test");
    }
    self.i++;
  }
  return list;
}

fn void Lexer.destroy(Lexer* self) {
  self.unimplemented();
}

fn void Lexer.unimplemented(Lexer* self) {
  io::print("error: [lexer] unimplemented");
  libc::exit(1);
}
