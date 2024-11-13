import types;

fn void slice_tests() @test
{
  Slice(<char>) my_slice = Slice(<char>){
    "hello world",
    0
  };

  assert(my_slice.equals("hello", 5), "slice |hello world| equals [hello, 5]");
}
