module ney;

//
import libc;
import utils;
import lexer;
import parser;
import std::io;
import std::collections;
import ast;
//

const BASE_SIZE = 16 * 1024;

fn int main(String[] args)
{
  String test_string = "u64 i = 5";
  Lexer lexer = lexer::init({test_string, test_string.len});
  lexer.lex();
  @pool() {
    Parser parser = parser::init(&lexer.tokens);
    parser.parse();
  };
  return 0;
}
