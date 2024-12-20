module ast;
import utils;
import lexer;
import types;
import std::io;
import std::core::mem;
import std::collections::list;

enum AstKind {
  FNCALLPARAMS, BLOCK, IMPORT,
  VARDECL, FOR, WHILE, FOREACH,
  RETURN, CONTINUE, BREAK //...
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

struct Condition {
  Ast* condition;
  Ast* block;
}

struct FuncParams {
  List(<Ast*>) list;
}

struct Match {
  Ast* symbol;
  List(<Condition>) conds;
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

struct Ast {
  AstKind kind;
  Location loc;
  Slice(<char>) slice;
  union {
    Expression expr;
    Literal literal;
    VarDecl var_decl;
    TypeDecl type_decl;
    FuncParams fn_params;
  }
}

fn Ast* create_decl(
  Slice(<char>) varname,
  Slice(<char>) typename,
  Ast* value,
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

fn Ast* create_function_call(...) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* create_binexpr(Ast* lhs, Ast* rhs, Token op) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* create_number(Token token) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* create_variable(Token token) {
  //TODO:
  Ast* ast = mem::new(Ast);
  return ast;
}

fn Ast* create_fnc_params(List(<Ast*>)* list) {
  Ast* ast = mem::new(Ast);
  ast.kind = AstKind.FNCALLPARAMS;
  ast.fn_params.list = *list;
  //TODO:
  return ast;
}
