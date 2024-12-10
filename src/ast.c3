module ast;
import utils;
import lexer;
import types;
import std::core::mem;
import std::collections::list;

enum AstKind {
  BLOCK, IMPORT, VARDECL, FOR, WHILE,
  FOREACH, RETURN, CONTINUE,
  BREAK //...
}

enum ExprKind {
  BINEXPR, UNARY, LITERAL, ACCESS,
  CAST, MATCH, IFSTMT
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

struct Case {
  Ast* condition;
  Ast* block;
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
  AstUnion unyon;
  Location loc;
  Slice(<char>) slice;
  union {
    Expression expr;
    Literal literal;
    VarDecl var_decl;
    TypeDecl type_decl;
  }
}

fn Ast* create_decl(
  Slice(<char>) varname,
  Slice(<char>) typename,
  Ast* value,
  bool is_primitive = false,
  Ast* base_value = null)
{
  Ast* ast = mem::new(Ast); //TODO:
  ast.kind = AstKind.VARDECL;

  ast.var_decl.name = varname;
  ast.var_decl.has_value = value != null;
  ast.var_decl.value = value;
  ast.var_decl.base_value = base_value;

  //ast.unyon.var_decl.type = Type{
  //  .name = typename,
  //  .is_primitive = is_primitive
  //};

  return ast;
}
