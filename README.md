# Ney

ney is an attempt at making a c like low level programming language but with life easiness and syntactic sugar of a high level programming language, ney is still in early stage development ! .

syntax examples

```c 
const char* str_literal = "hello"; // same
const string str_literal = "hello"; // same
const uint8 number = 5;
// not same as
immut uint8 number = 5; // immut cannot be changed with the address of the variable attempting to dereference will result in a dereferencing immutable variable exception
const string str_literal = "hello"; // same as immut since string literals are read only << and cannot be changed and dereferencing a string literal could lead to a segfault
immut string str_literal = "hello";

// second case will be handled by the compiler
// both are will be stored in the heap of the current scope

func do_thing() : uint8 {
  uint8 unsigned_integer = do_something_that_returns_u8();
  return unsigned_integer;
}

uint8 unsigned_integer = () => {
  return do_something_that_returns_u8();
};

// both equivalent

```

