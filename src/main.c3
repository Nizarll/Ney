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

fn void print_usage() {
  io::print(
    "usage: ney [file] [options].\n\n"
    "Options:\n"
    "-eo: enable optimizations\n"
    "-w: enable warnings\n"
    "-bc: outputs a compiled bytecode file\n"
    "-o: specify the name of the output\n"
  );
}

struct Params {
  void* params;
}

fn Params parse_args(String[] args) {
  if (args.len <= 1) print_usage();
  //TODO: parse parameters
  return {null};
}

fn int main(String[] args)
{
  Params params = parse_args(args);

  String! buffer = (String) io::file::load_new(args[1]);
  if (catch excuse = buffer) {
    io::print("[error]: file does not exist\n");
    libc::exit(libc::EXIT_FAILURE);
  }

  Lexer lexer = lexer::init(utils::string_to_slice(buffer!!));
  lexer.lex();
  @pool() {
    Parser parser = parser::init(&lexer.tokens);
    parser.parse();
  };
  return 0;
}
