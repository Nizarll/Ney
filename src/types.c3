module types;
import utils;

enum DefaultType {
  CHAR, BOOL, FUNC, STR,
  NUMBER, LAMBDA, STRUCT,
  ARRAY, ERROR
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

fn String Type.to_str() {
  //TODO:
  return "UNIMPLEMENTED TYPE.TO_STR()";
}
