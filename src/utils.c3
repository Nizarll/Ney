def Location = usz[<2>];

module utils::maybe(<Type>);
import std::core::types;
struct Maybe {
  Type data;
  String message;
}

macro bool Maybe.err(&self) {
  $switch(Type.kindof)
    $case(TypeKind.BOOL):
      return self.data == false;
    $default:
      $if (types::is_numerical(Type)):
        return self.data == 0;
      $else
        return self.data == null;
      $endif
  $endswitch
}


macro Maybe(<Type>) unexpected(String message) {
  $switch(Type.kindof)
    $case(TypeKind.BOOL):
      return Maybe(<Type>){false, message};
    $default:
      $if (types::is_numerical(Type)):
        return Maybe(<Type>){0, message};
      $else
        return Maybe(<Type>){null, message};
      $endif
  $endswitch
}

//
module utils::string;
import utils::slice;

fn Slice(<char>) String.to_slice(self) {
  return Slice(<char>){self, self.len};
}

//
module utils::slice(<Type>);
import libc;


struct Slice {
  Type* ptr;
  usz len;
}

fn Type Slice.get(Slice* self, usz id = 0) @operator([]) {
  return (Type)(*(self.ptr + id));
}

fn Type* Slice.get_ref(Slice* self, usz id = 0) @operator(&[]) {
  return (Type*)(self.ptr + id);
}

fn usz Slice.len(Slice* self) @operator(len) {
  return self.len;
}

fn bool Slice.equals(Slice* self, Type* other) {
  return (bool) (libc::memcmp(self.ptr, other, self.len) == 0);
}
