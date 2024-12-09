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
    "-eo, --enableopts: enable optimizations.\n"
    "-w, --warnings: enable warnings\n"
    "-h, --help: display warnings.\n"
    "-bc, --bytecode: outputs a compiled bytecode file.\n"
    "-o, --output: specify the name of the output.\n\n"
  );
  libc::exit(libc::EXIT_FAILURE);
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

  Lexer lexer = lexer::init((buffer!!).to_slice());
  lexer.lex();
  @pool() {
    Parser parser = parser::init(&lexer.tokens);
    parser.parse();
  };
  return 0;
}
