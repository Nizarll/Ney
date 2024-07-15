# Ney

ney is an attempt at making a c like low level programming language but with life easiness and syntactic sugar of a high level programming language, ney is still in early stage development ! .

✓ parser \
✓ lexer \
⨯ code generator \
⨯ implement llvm backend  \

the goal of this project is to have some code like this up and running:

```c
import cstd;

struct task {
  state s;
  str content;
};

struct tasks {
  task* buff;
  u8 _len; // private member
  u8 _curr;
};

fn task::new(str content) {
  return task{.content = content};
}

fn task::copy(task* self) -> task {
  task t;
  return cstd::memcopy(&t, self, sizeof(task)); // deep copy using the c memcopy function
}

fn tasks::push(tasks* self, task t) {
  self->buff[self->_curr++] = t::copy();
}

fn main() -> int {
  tasks t;
  t.push(task.new("hello"));
  return 0;
}
```
