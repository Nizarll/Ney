def Location = usz[<2>];

module utils::slice(<Type>);
import libc;


struct Slice {
  Type* view;
  usz len;
}

fn Type Slice.get(Slice* self, usz id = 0) @operator([]) {
  return (Type)(*(self.view + id));
}

fn Type* Slice.get_ref(Slice* self, usz id = 0) @operator(&[]) {
  return (Type*)(self.view + id);
}

fn usz Slice.len(Slice* self) @operator(len) {
  return self.len;
}

fn bool Slice.equals(Slice* self, Type* other, usz size) {
  return (bool) (libc::memcmp(self.view + self.len, other, size) == 0);
}

import types;
module utils::ast;
import utils::slice;

enum AstType {
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
  AstType type;
  AstUnion node;
  Slice(<char>) slice;
  Location slice;
}
