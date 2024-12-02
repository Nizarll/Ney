module ney;

//
import libc;
import utils;
import lexer;
import parser;
//
import std::io;
import std::collections;
//

fn int main(String[] args)
{
  String test_string = 
  "fn main()"
  "u64 i = 5"
  "print(i)"
  "end";
  Lexer lexer = lexer::init({test_string, test_string.len});
  lexer.lex();
  Parser parser = parser::init(&lexer.tokens);
  parser.parse();
  return 0;
}
