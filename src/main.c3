module ney;

//
import libc;
import types;
import lexer;
import std::io;
//

fn int main(String[] args)
{
  String test_string = "struct test";
  io::print(test_string.len);
  Lexer lexer = lexer::init(
        Slice(<char>) {
          test_string, test_string.len
        }
  );
  lexer.lex();
  return 0;
}
