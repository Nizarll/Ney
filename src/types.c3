module types::slice(<Type>);
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
