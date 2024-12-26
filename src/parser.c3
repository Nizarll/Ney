import libc;
import ast;
import std::io;
import types;
import lexer;
import utils;
import std::collections::list;

struct Ns {
  void* todo;
}

struct Parser {
  usz i;
  List(<Token>)* tokens;
  List(<Ast*>) nodes;
  List(<Ns>) nspcs;
  List(<Func>) fns;
}

struct Func {
  Ast* args;
  Ast* body;
  Type ret_type;
  Slice(<char>) name;
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

const int IF = 0x00;
const int ELSEIF = 0x01;
const int ELSE = 0x02;

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
  while (!self.is_eof()) {
    
    if (self.is_proto()) {
      self.parse_fn();
    } else if (self.is_stmt()) {
      Maybe(<Ast*>) ast = self.parse_stmt();
      if (ast.err()) {
        io::printf("error: %s \n", ast.message);
        return;
      }
      self.nodes.push(ast.data);
    }

  }
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

fn bool Parser.is_proto(&self) {
  //TODO:
  TKind[*] kinds = {TKind.SYMBOL, TKind.SYMBOL, TKind.OPENPAREN};
  for (int i = 0; i < kinds.len; i++) {
    if (!self.peek(i).is(kinds[i])) return false;
  }
  return true;
}

fn bool Parser.is_call(&self) {
  //TODO:
  TKind[*] kinds = {TKind.SYMBOL, TKind.OPENPAREN};
  for (int i = 0; i < kinds.len; i++) {
    if (!self.peek(i).is(kinds[i])) return false;
  }
  return true;
}

fn bool Parser.is_lit(&self, uint offset = 0) {
  return (self.peek(2).is(TKind.STR) ||
  self.peek(2).is(TKind.CHAR)    ||
  self.peek(2).is(TKind.CHAR)    ||
  self.peek(2).is(TKind.NUMLIT));
}

fn bool Parser.is_stmt(&self) {
  if (self.token().is(TKind.SYMBOL) && self.match("if")) { return true; }
  if (self.token().is(TKind.SYMBOL) && self.match("elseif")) { return true; }
  if (self.token().is(TKind.SYMBOL) && self.match("else")) { return true; }
  if (self.token().is(TKind.SYMBOL) && self.peek().is(TKind.SYMBOL)) { return true; }
  return false;
}

fn Maybe(<Ast*>) Parser.parse_stmt(&self) {
  
  if (self.token().is(TKind.SYMBOL) && self.match("if")) { return self.parse_ifstmt(IF); }
  if (self.token().is(TKind.SYMBOL) && self.match("elseif")) { return self.parse_ifstmt(ELSEIF); }
  if (self.token().is(TKind.SYMBOL) && self.match("else")) { return self.parse_ifstmt(ELSE); }
  if (self.token().is(TKind.SYMBOL) && self.peek().is(TKind.SYMBOL)) { return self.parse_decl(); }

  _unimplemented("unexpected statement in parse_stmt!");

  return maybe::unexpected(<Ast*>)("Unimplemented parse statement !!");
}

fn Maybe(<Ast*>) Parser.parse_ifstmt(&self, int variant) {
  Maybe(<Ast*>) parse_error, expected_block_error;

  parse_error = maybe::unexpected(<Ast*>)("failed to parse if statement");
  expected_block_error = maybe::unexpected(<Ast*>)("expected if or elseif statement");
  
  if (!self.match(variant == IF ? "if": variant == ELSEIF ? "elseif" : "else")) return parse_error;

  if (variant != IF) {
    Ast*! last = self.nodes.last();
    if (catch excuse = last) {
      return expected_block_error;
    }
    if (!last.is_ifblock()) return expected_block_error;
  }

  parse_error = self.consume(TKind.SYMBOL, "failed to parse if statement");
  if (parse_error.err()) return parse_error;
  
  Maybe(<Ast*>) expr;
  if (variant != ELSE) {
    expr = self.parse_expr(TKind.OPENCURLY);
    if (expr.err()) return expr;
  }

  Ast* node = ast::make_ifstmt(expr.data, variant);
  Maybe(<Ast*>) block = self.parse_block();
  
  if (block.err()) {
    //DEBUG
    return block; 
  }
  node.set_block(block.data);
  
  return Maybe(<Ast*>){node, ""};
}

fn Maybe(<Ast*>) Parser.parse_else(&self) {
  return Maybe(<Ast*>){null, ""};
}

fn Maybe(<Ast*>) Parser.parse_block(&self) {
  Maybe(<Ast*>) parse_error;
  parse_error = self.consume(TKind.OPENCURLY, "expected curly bracket after condition in if statement");
  if (parse_error.err()) return parse_error;

  Ast* block = ast::make_block();
  while (!self.token().is(TKind.CLOSECURLY)) {
    Maybe(<Ast*>) stmt = self.parse_stmt();
    if (stmt.err()) return stmt;
    block.push_stmt(stmt.data);
  }

  parse_error = self.consume(TKind.CLOSECURLY, "expected closing curly bracket at the end of if statement");
  if (parse_error.err()) return parse_error;


  return {block, ""};
}

fn void Parser.parse_fn(&self) {
  Maybe(<Ast*>) consume;

  Token type = self.token();
  consume = self.consume(TKind.SYMBOL, "Expected typename in function declaration");
  if (consume.err()) {
    io::print(consume.message);
    io::print("func todo remove\n");
  }

  Token name = self.token();
  consume = self.consume(TKind.SYMBOL, "Expected function name after typename in function declaration");
  if (consume.err()) {
    io::print(consume.message);
    io::print("func todo remove\n");
  }

  consume = self.consume(TKind.OPENPAREN, "Expected an opening parenthesis after function name"); // JUST TO BE SURE
  if (consume.err()) {
    io::print(consume.message);
    io::print("func todo remove\n");
  }

  io::print("parsed first part\n");
  io::print("\n");

  Maybe(<Ast*>) args = self.parse_fn_args();
  io::print("finished parsing args ! \n");

  Maybe(<Ast*>) block = self.parse_block();

  if (block.err()) {
    io::print("error in bug or whatever in function you gotta fix this later\n"); // TODO: //DEBUG
    libc::exit(1);
  }

  Func fnc = Func {
    //.type  TODO: add type
    .name = name.view,
    .args = args.data,
    .body = block.data
  };
  
  //DEBUG
  libc::printf("parsed function %.*s\n", name.view.len, name.view.ptr);
  //DEBUG
  
  self.fns.push(fnc);
}

fn Maybe(<Ast*>) Parser.parse_decl(&self) {
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
      _unimplemented("handle value of expr in equal");
    }

    ast = ast::make_decl(
      name_tok.view,
      primitive_type.to_slice(),
      value.data,
      ast::PRIMITVE_TYPE
    );
    
  } else {
    ast = ast::make_decl(
      name_tok.view,
      primitive_type.to_slice()
    );
  }
  
  Maybe(<Ast*>) consume = self.consume(TKind.SEMICOL, "expected a semicolon after declaration ");
  if (consume.err()) return consume;
  
  return Maybe(<Ast*>){ast, ""};
}

fn Maybe(<Ast*>) Parser.parse_expr(&self, TKind until = TKind.SEMICOL) {
  Maybe(<Ast*>) left = self.parse_term();
  if (left.err()) return left;

  while (self.is_comparison() && !self.token().is(until)) {
    Token op = self.token();
    self.inc();

    Maybe(<Ast*>) right = self.parse_term(until);
    if (right.err()) return right;

    left = Maybe(<Ast*>) {
      ast::make_binexpr(left.data, right.data, op),
      ""
    };
  }
  return left;
}

fn Maybe(<Ast*>) Parser.parse_term(&self, TKind until = TKind.SEMICOL) {
  Maybe(<Ast*>) left = self.parse_factor();
  if (left.err()) return left;

  while (self.token().is(TKind.PLUS) || self.token().is(TKind.MINUS) && !self.token().is(until)) {
    Token op = self.token();
    self.inc();

    Maybe(<Ast*>) right = self.parse_factor(until);
    if (right.err()) return right;

    left = {
      ast::make_binexpr(left.data, right.data, op),
      ""
    };
  }
  return left;
}

fn Maybe(<Ast*>) Parser.parse_factor(&self, TKind until = TKind.SEMICOL) {
  Maybe(<Ast*>) left = self.parse_primary();
  if (left.err()) return left;

  while ((self.token().is(TKind.MUL) || self.token().is(TKind.DIV)) && !self.token().is(until)) {
    Token op = self.token();
    self.inc();

    Maybe(<Ast*>) right = self.parse_factor(until);
    if (right.err()) return right;

    left = Maybe(<Ast*>) {
      ast::make_binexpr(left.data, right.data, op),
      ""
    };
  }

  return left;
}

fn Maybe(<Ast*>) Parser.parse_primary(&self) { //TODO: refactor this shit its horrible
  Maybe(<Ast*>) left;

  if (self.match("match")) {
    return self.parse_match();
  }

  if (self.token().is(TKind.NUMLIT)) {
    Token num = self.token();
    self.inc();
    return Maybe(<Ast*>) { ast::make_number(num), "" };
  }
  if (self.token().is(TKind.SYMBOL)) {
    Token id = self.token();
    self.inc();

    if (self.token().is(TKind.OPENPAREN)) {
      // Handle function call
      self.inc();
      Maybe(<Ast*>) args = self.parse_call_args();
      if (args.err()) return args;

      if (!self.token().is(TKind.CLOSEPAREN)) {
        return maybe::unexpected(<Ast*>)("Expected closing parenthesis after function call");
      }

      self.inc();
      left = Maybe(<Ast*>) { ast::make_function_call(id, args), "" };
    } else {
      left = Maybe(<Ast*>) { ast::make_variable(id), "" };
    }

  } else if (self.token().is(TKind.OPENPAREN)) {
    self.inc();
    left = self.parse_expr(TKind.CLOSEPAREN);
    if (left.err()) return left;

    if (!self.token().is(TKind.CLOSEPAREN)) {
      return maybe::unexpected(<Ast*>)("Expected closing parenthesis after expression");
    }

    self.inc();
  } else {
    return maybe::unexpected(<Ast*>)("Unexpected token in primary expression");
  }

  return left;
}

fn Maybe(<Ast*>) Parser.parse_call_args(&self) {
  //TODO:parse arguments depending on function to give better errors
  Ast* args_ast;
  List(<Ast*>) params;
  while (!self.token().is(TKind.CLOSEPAREN)) {
    Maybe(<Ast*>) expr = self.parse_expr();
    
    if (expr.err()) return expr;
    params.push(expr.data);

    if (self.token().is(TKind.CLOSEPAREN)) {
      continue;
    } else if (self.token().is(TKind.COMA)) {
      self.inc();
    } else {
      return maybe::unexpected(<Ast*>)("Unexpected token here");
    }
  }

  args_ast = ast::make_call_args(&params);
  return Maybe(<Ast*>){ args_ast, "" };
}

fn Maybe(<Ast*>) Parser.parse_fn_args(&self) {
  Maybe(<Ast*>) consume;

  Ast* ast = ast::make_fn_args();
  
  while(!self.token().is(TKind.CLOSEPAREN)) {
    Token type = self.token();
    // TODO: add typechecking so that if it a correct type within the namespace correct error to expected name after type 
    consume = self.consume(TKind.SYMBOL, "Expected type in function parameter");
    if (consume.err()) return consume;

    Token name = self.token();
    consume = self.consume(TKind.SYMBOL, "Expected name after type in function parameter");
    if (consume.err()) return consume;

    if (self.token().is(TKind.COMA)) {
      consume = self.consume(TKind.COMA, "Expected name after type in function parameter");
      if (consume.err()) return consume;
    }
    //TODO: ast.push_fn_argument(type, name);
  }
  consume = self.consume(TKind.CLOSEPAREN, "Expected closing parenthesis after function arguments");
  if (consume.err()) return consume;

  return {ast, ""};
}

fn Maybe(<Ast*>) Parser.parse_match(&self) {
  Maybe(<Ast*>) consume;
  self.token().print();
  io::print("\n\n");
  consume = self.consume(TKind.SYMBOL, "Expected a match keyword in match statement"); // ?? just for safety.
  
  Token name = self.token();
  consume = self.consume(TKind.SYMBOL, "Expected variable name in match statement"); // ?? just for safety.
  consume = self.consume(TKind.OPENCURLY, "Expected a curly bracket after variable name in match statement");

  Ast* match = ast::make_match();

  while (!self.token().is(TKind.CLOSECURLY)) {
    // parse cases
    Maybe(<Ast*>) expr = self.parse_expr(TKind.ARROW);
    if (expr.err()) return expr;
    
    char[1024] buffer;
    char[] sliced = buffer[..1023];

    io::bprintf(sliced, "expected an arrow ( %s => result ) after expression in match statement !");
    consume = self.consume(TKind.ARROW, (String) sliced);
    if (self.token().is(TKind.OPENCURLY)) {
      Maybe(<Ast*>) block = self.parse_block();
      //match.add_branch(ast::BLOCK, block.data); TODO:
    } else {
      Maybe(<Ast*>) result = self.parse_expr(TKind.COMA);
      if (result.err()) return result;
      consume = self.consume(TKind.COMA, "");
      if (consume.err() && !self.peek().is(TKind.CLOSECURLY)) {
        return maybe::unexpected(<Ast*>)("expected closing curly after last branch of match statement !");
      } else if(consume.err()) {
        return maybe::unexpected(<Ast*>)("expected comma after branch of match statement !");
      }
      //ast.add_branch(ast::EXPR, result.data); TODO:
    }
  }

  consume = self.consume(TKind.CLOSECURLY, "expected closing curly bracket at the end of match statement !");
  if (consume.err()) return consume;

  return {match, ""};
}

macro _unimplemented(String s = "") {
  libc::printf("[Parser]: Unimplemented %.*s At: %s Line %d\n", s.len, s, $$FUNC, $$LINE);
  libc::exit(1);
}
