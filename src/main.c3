module ney;

//
import libc;
import types;
import lexer;
import std::io;
import std::collections;
//

fn int main(String[] args)
{

  String test_string = ">= <= =>";
  Lexer lexer = lexer::init(
        Slice(<char>) {
          test_string, test_string.len
        }
  );
  List(<Token>) list = lexer.lex();
  foreach(&entry : list) {
    entry.print();
    io::print(" ");
  }
  io::print("\n");
  return 0;
}
