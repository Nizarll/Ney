def Location = usz[<2>];

module utils::maybe(<Type>);
struct Maybe {
  Type data;
  String message;
}

fn bool Maybe.err(&self) @inline {
  return self.data == null;
}

fn Maybe(<Type>) unexpected(String message) {
  return Maybe(<Type>){null, message};
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
