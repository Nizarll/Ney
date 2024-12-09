module types;
import utils;
import parser;

enum DefaultType {
  BOOL, FUNC, STR,
  U64, U32, U16, CHAR,
  LAMBDA, STRUCT,
  ARRAY, F32, F64,
}

struct Struct {
  Slice(<Type>) members;
}

struct Func {
  Type* rtype;
  Slice(<Type>) params;
}

struct TypeUnion {
  Struct struc;
  Func func;
}

struct Type {
  String name;
  DefaultType default_type;
  TypeUnion type_union;
}

fn String Type.to_str(&self) {
  parser::_unimplemented("UNIMPLEMENTED TYPE.TO_STR()");
  return "";
}

