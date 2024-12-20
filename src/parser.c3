import libc;
import ast;
import std::io;
import types;
import lexer;
import utils;
import std::collections::list;

fault ParserFaults {
  OUT_OF_BOUNDS,
}

struct Ns {
  void* todo;
}

struct Parser {
  usz i;
  List(<Token>)* tokens;
  List(<Ast*>) nodes;
  List(<Ns>) namespaces;
}

String[*] keywords = {
  "let", "fn", "as", "end", "match",
  "then", "for", "foreach", "while",
  "len", "typeof", "if", "elseif", "else",
  "import", "export", "do",
};

String[*] types = {
  "uint", "int", "float", "double",
  "u64", "u32", "u16", "u8",
  "f32", "f64", "str", "char",
  "bool",
};

fn Parser init(List(<Token>)* tokens) {
  Parser p;
  p.i = 0;
  p.tokens = tokens;
  return p;
}

fn Token Parser.token(&self) @inline {
  return self.tokens.get(self.i);
}

fn Token Parser.peek(&self, int offset = 1) {
  if (self.i + offset < 0) return Token{TKind.EOF, {}, {}};
  if (self.i + offset >= self.tokens.len()) return Token{TKind.EOF, {}, {}};
  return self.tokens.get(self.i + offset);
}

fn bool Parser.is_token(&self, TKind kind) @inline {
  return (self.tokens.len() != 0 && self.i < self.tokens.len() && self.token().kind == kind);
}

fn bool Parser.is_eof(&self) @inline {
  return (self.tokens.len() == 0 || self.token().kind == TKind.EOF || self.i >= self.tokens.len());
}

fn void Parser.inc(&self) @inline {
  if (self.i >= self.tokens.len()) return;
  self.i += 1;
}

fn bool Parser.match(&self, String s) @inline {
  Slice(<char>) view = self.token().view;
  if (view.len == s.len) {
    return libc::memcmp(self.token().view.ptr, (char*) s, s.len) == 0;
  }
  return false;
}

fn void Parser.parse(&self) {
 // foreach(&token : self.tokens) {
 //   token.print();
 // }
 // self.i = 0;
 io::print("\nparsing");
  while (!self.is_eof()) {
    self.token().print();
    if (self.is_stmt()) self.parse_stmt();
  }
}

fn bool Parser.is_stmt(&self) {
  if (self.token().is(TKind.SYMBOL) && self.match("if")) {
    io::print("is_stmt: matched if stmt\n");
    return true;
  }
  if (self.token().is(TKind.SYMBOL) && self.peek().is(TKind.SYMBOL)) return true;
  //if (self.is_ns_type()) return true;
  //if (self.is_assign()) return true;
  //if (self.is_call()) return true;
  return false;
}

fn String* Parser.get_primitive_type(&self) {
  foreach(&type : types) {
    if (self.match(*type)) return type;
  }
  return null;
}

macro Maybe(<Ast*>) Parser.consume(&self, TKind kind, String message) {
  if (self.token().is(kind)) {
    self.inc();
    return {(Ast*)1, ""};
  }
  io::printf("expected %s but found %s Func: %s ,At Line %d\n", kind.tostr, self.token().tostr(), $$FUNC, $$LINE);
  return maybe::unexpected(<Ast*>)(message);
}

fn bool Parser.is_ns_type(&self) {
  //TODO: 
  //todo do this shit im too lazy rn im ngl
  return false;
}

fn bool Parser.is_assign(&self) {
  TKind[*] kinds = {TKind.SYMBOL, TKind.EQ};
  //TODO: refactor this bullshit im lazy af
  foreach (kind : kinds) {
    if (!self.token().is(kind)) {
      return false;
    }
  }
  return true;
}

fn bool Parser.is_comparison(&self) @inline {
  return (self.token().is(TKind.ISEQ)   || self.token().is(TKind.NOTEQ)  ||
  self.token().is(TKind.LESSEQ) || self.token().is(TKind.GREATEREQ) ||
  self.token().is(TKind.LESS)   || self.token().is(TKind.GREATER));
}

fn bool Parser.is_call(&self) {
  //TODO:
  TKind[*] kinds = {TKind.SYMBOL, TKind.OPENPAREN};
  foreach (kind : kinds) {
    if (!self.token().is(kind)) return false;
  }
  return false;
}

fn bool Parser.is_lit(&self, uint offset = 0) {
  return (self.peek(2).is(TKind.STR) ||
  self.peek(2).is(TKind.CHAR)    ||
  self.peek(2).is(TKind.CHAR)    ||
  self.peek(2).is(TKind.NUMLIT));
}

fn void Parser.parse_stmt(&self) {
  if (self.token().is(TKind.SYMBOL) && self.match("if")) {
    self.parse_ifstmt();
    return;
  }
  if (self.token().is(TKind.SYMBOL) && self.peek().is(TKind.SYMBOL)) {
    self.parse_decl();
    return;
  }
  _unimplemented("unexpected statement in parse_stmt!");
  //if (self.is_ns_type());
  //if (self.is_assign());
  //if (self.is_call());
}

fn Maybe(<Ast*>) Parser.parse_ifstmt(&self) {
  Maybe(<Ast*>) ast;
  if (!self.match("if")) return maybe::unexpected(<Ast*>)("failed to parse if statement");

  ast = self.consume(TKind.SYMBOL, "failed to parse if statement");
  if (ast.err()) return ast;
  
  Maybe(<Ast*>) expr = self.parse_expr();
  
  while (!self.match("end")) {
    self.inc();
    //TODO: parse statements here
  }

  return Maybe(<Ast*>){null, ""};
}

fn void Parser.parse_match_stmt(&self) {
  _unimplemented();
}

fn void Parser.parse_decl(&self) {
  Ast *ast;
  String* primitive_type = self.get_primitive_type();

  Token name_tok = self.peek();
  if (!name_tok.is(TKind.SYMBOL)) {
    _unimplemented("EXPECTED VARIABLE NAME + TODO: add correct error system");
  }
  self.inc();

  if (!primitive_type) {
    _unimplemented("HANDLE USER DEFINED TYPES");
  }

  self.inc();
  if (self.token().is(TKind.EQ)) {
    
    self.inc(); // GO BEYOND EQUAL

    Maybe(<Ast*>) value = self.parse_expr();

    //TODO: remove this
    if (value.err()) {
      io::print(value.message);
      io::print("\n");
      _unimplemented("handle value of expr in equal");
    }

    ast = ast::create_decl(
      name_tok.view,
      primitive_type.to_slice(),
      value.data,
      ast::PRIMITVE_TYPE
    );
    
  } else if (self.peek().is(TKind.EQ)) {
    _unimplemented("user defined type after equal");
  }
}

fn Maybe(<Ast*>) Parser.parse_expr(&self) {
  Maybe(<Ast*>) left = self.parse_term();
  if (left.err()) return left;

  while (self.is_comparison()) {
    Token op = self.token();
    self.inc();

    Maybe(<Ast*>) right = self.parse_term();
    if (right.err()) return right;

    left = Maybe(<Ast*>) {
      ast::create_binexpr(left.data, right.data, op),
      ""
    };
  }
  return left;
}

fn Maybe(<Ast*>) Parser.parse_term(&self) {
  Maybe(<Ast*>) left = self.parse_factor();
  if (left.err()) return left;

  while (self.token().is(TKind.PLUS) || self.token().is(TKind.MINUS)) {
    Token op = self.token();
    self.inc();

    Maybe(<Ast*>) right = self.parse_factor();
    if (right.err()) return right;

    left = {
      ast::create_binexpr(left.data, right.data, op),
      ""
    };
  }
  return left;
}

fn Maybe(<Ast*>) Parser.parse_factor(&self) {
  Maybe(<Ast*>) left;
//TODO: refactor this ugly ass shit
  if (self.token().is(TKind.NUMLIT)) {
    Token num = self.token();
    self.inc();
    left = Maybe(<Ast*>) { ast::create_number(num), "" };
  } else if (self.token().is(TKind.SYMBOL)) {
    Token id = self.token();
    self.inc();

    if (self.token().is(TKind.OPENPAREN)) {
      // Handle function call
      self.inc();
      Maybe(<Ast*>) args = self.parse_arguments();
      if (args.err()) return args;

      if (!self.token().is(TKind.CLOSEPAREN)) return maybe::unexpected(<Ast*>)("Expected closing parenthesis");

      self.inc();
      left = Maybe(<Ast*>) { ast::create_function_call(id, args), "" };
    } else {
      left = Maybe(<Ast*>) { ast::create_variable(id), "" };
    }
  } else if (self.token().is(TKind.OPENPAREN)) {
    self.inc();
    left = self.parse_expr();
    if (left.err()) return left;

    if (!self.token().is(TKind.CLOSEPAREN)) return maybe::unexpected(<Ast*>)("Expected closing parenthesis");
    self.inc();
  } else {
    return maybe::unexpected(<Ast*>)("Unexpected token in factor");
  }

  // Handle multiplication and division
  while (self.token().is(TKind.MUL) || self.token().is(TKind.DIV)) {
    Token op = self.token();
    self.inc();

    Maybe(<Ast*>) right = self.parse_factor();  // Recursively parse the right-hand side
    if (right.err()) return right;

    left = Maybe(<Ast*>) {
      ast::create_binexpr(left.data, right.data, op),
      ""
    };
  }

  return left;
}

fn Maybe(<Ast*>) Parser.parse_arguments(&self) {
  //TODO:parse arguments depending on function to give better errors
  Ast* args_ast;
  List(<Ast*>) params;
  while (!self.token().is(TKind.CLOSEPAREN)) {
    Maybe(<Ast*>) expr = self.parse_expr();
    
    if (expr.err()) return expr;
    params.push(expr.data);

    if (self.token().is(TKind.CLOSEPAREN)) {
      continue;
    } else if (self.token().is(TKind.COLON)) {
      self.inc();
    } else {
      return maybe::unexpected(<Ast*>)("Unexpected token here");
    }
  }

  args_ast = ast::create_fnc_params(&params);
  return Maybe(<Ast*>){ args_ast, "" };
}

macro _unimplemented(String s = "") {
  libc::printf("[Parser]: Unimplemented %.*s At: %s Line %d\n", s.len, s, $$FUNC, $$LINE);
  libc::exit(1);
}
