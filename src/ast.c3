module ast;
import utils;
import types;

enum AstKind {
  UNOP, BINOP, TYPEOF, BLOCK, IFSTM,
  IMPORT, LIT, VARDECL, FOR, WHILE,
  FOREACH, MATCH, CAST, RETURN, CONTINUE,
  BREAK //...
}

//Node Kind

struct TypeDecl {
  void* todo; // TODO
}

struct VarDecl {
  Type type;
  bool has_value;
  Ast* value;
  Ast* base_value; // default value in function declaration;
  Slice(<char>) name;
}

union AstUnion {
  VarDecl var_decl;
  TypeDecl type_decl;
}

struct Ast {
  AstKind kind;
  AstUnion unyon;
  Location loc;
  Slice(<char>) slice;
}
