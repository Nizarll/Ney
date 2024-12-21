module bytecode;

import std::collections::list;

enum Opcode : uint (String tostr) {
  LOAD = "load", MOV = "mov", ADD = "add",
  SUB = "sub", POP = "pop", PUSH = "push",
  CMP = "cmp", JMP = "jmp",
}

union Const {
  void* ptr;
  int i32;
  uint u32;
  float float32;
  double float64;
}

def ConstTable = List(<Const>) consts;

def GpRegs = uint[16]; // general purpose registers
def FpRegs = float[8]; // 8 floating point registers

struct Vm {
  Registers registers;
  ConstTable ctable;
  GpRegs general_regs;
  FpRegs floating_regs;
}
