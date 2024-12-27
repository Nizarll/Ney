module ast;
import utils;
import lexer;
import types;
import libc;
import std::io;
import std::core::mem;
import std::collections::list;

enum AstKind : uint (String tostr) {
  FNCALLPARAMS = "FNCALLPARAMS",
  BLOCK = "BLOCK", IMPORT = "IMPORT",
  FOR = "FOR", WHILE = "WHILE",
  FOREACH = "FOREACH", IF = "IF",
  VARDECL = "VARDECL", ELSE = "ELSE",
  ELSEIF = "ELSEIF", RETURN = "RETURN",
  CONTINUE = "CONTINUE", BREAK = "BREAK",
  MATCH = "MATCH",
}

enum ExprKind {
  BINEXPR, UNARY, LITERAL, ACCESS,
  CAST, MATCH,
}

enum BinopKind {
  SHIFTLEFT, SHIFTRIGHT, GREATEREQ,
  LESSEQ, ISEQ, NOTEQ, BXOR, BNOT, BOR, // BINARY OR
  OR
}

const bool PRIMITVE_TYPE = true;
const int EXPR  = 0x00;
const int BLOCK = 0x01;
//Node Kind

struct TypeDecl {
  void* todo; // TODO
}

struct VarDecl {
  Type type;
  bool has_value;
  Ast* value;
  Ast* base_value;
  Slice(<char>) name;
}

struct Literal {
  Type type;
  Ast* value;
}

struct Binexpr {
  Type type;
  Ast* lhs;
  Ast* rhs;
  BinopKind kind;
}

struct Ifstmt {
  Ast* cond;
  Ast* block;
}

struct Block {
  List(<Ast*>) stmts;
}

struct Condition {
  Ast* condition;
  Ast* block;
}

struct FuncParams {
  List(<Ast*>) list;
}

struct Match {
  Ast* symbol;
  List(<Ast*>) cases;
}

struct Expression {
  //TODO:
  Type type;
  union {
    Literal lit;
    Match match;
    Binexpr binexpr;
  }
}

struct FuncCall {
  Token id;
  List(<Ast*>) args;
}

struct Ast {
  AstKind kind;
  Location loc;
  Slice(<char>) slice;
  union {
    Block block;
    Ifstmt ifstmt;
    Expression expr;
    Literal literal;
    VarDecl var_decl;
    TypeDecl type_decl;
    FuncCall call;
    FuncParams fn_params;
  }
}

fn Ast* make_decl(
  Slice(<char>) varname,
  Slice(<char>) typename,
  Ast* value = null,
  bool is_primitive = false,
  Ast* base_value = null)
{

  Ast* ast = mem::new(Ast);
  ast.kind = AstKind.VARDECL;

  ast.var_decl.name = varname;
  ast.var_decl.has_value = value != null;
  ast.var_decl.value = value;
  ast.var_decl.base_value = base_value;

  //TODO:
  
  //ast.unyon.var_decl.type = Type{
  //  .name = typename,
  //  .is_primitive = is_primitive
  //};

  return ast;
}

fn Ast* make_function_call(Token id, Ast* args) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* make_binexpr(Ast* lhs, Ast* rhs, Token op) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* make_number(Token token) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* make_variable(Token token) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* make_fn_args() {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* make_call_args(List(<Ast*>)* list) {
  Ast* ast = mem::new(Ast);
  ast.kind = AstKind.FNCALLPARAMS;
  ast.fn_params.list = *list;
  //TODO:
  return ast;
}

fn Ast* make_ifstmt(Ast* cond, int variant) {
  Ast* ast = mem::new(Ast);
  if (variant == 0) {
    ast.kind = AstKind.IF;
  } else if (variant == 1) {
    ast.kind = AstKind.ELSEIF;
  } else if (variant == 2) {
    ast.kind = AstKind.ELSE;
  }
  return ast;
}

fn Ast* make_block() {
  Ast* ast = mem::new(Ast);
  ast.kind = AstKind.BLOCK;
  return ast;
}

fn Ast* make_match() {
  Ast* ast = mem::new(Ast);
  ast.kind = AstKind.MATCH;
  return ast;
}

fn bool Ast.is_ifblock(&self) @inline {
  return (self.kind == AstKind.IF || self.kind == AstKind.ELSEIF);
}

fn bool Ast.is_elseblock(&self) @inline {
  return (self.kind == AstKind.ELSE);
}

fn void Ast.set_block(&self, Ast* ast) @inline {
  if (self.is_elseblock() || self.is_ifblock()) {
    self.ifstmt.block = ast;
  } else {
    io::printf("unimplemented ast.setblock for : %s\n", self.kind.tostr); //TODO: unimplemented
  }
}

fn void Ast.push_stmt(&self, Ast* ast) @inline {
  if (ast == null) {
    io::print("attempted to push a null statement to block\n");
    libc::exit(1);
  }
  self.block.stmts.push(ast);
}
