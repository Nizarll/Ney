def Location = usz[<2>];

import utils::slice;
fn Slice(<char>) string_to_slice(String s) {
  return Slice(<char>){s, s.len};
}

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
