module bytecode;

import libc;
import std::collections::list;
import std::io;

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

def ConstTable = List(<Const>);

def GpRegs = uint[16]; // general purpose registers
def FpRegs = float[8]; // 8 floating point registers
def Instruction = uint;

struct Vm {
  ConstTable ctable;
  GpRegs general_regs;
  FpRegs floating_regs;
}

struct VmContext {
  Vm* vm;
  uint pc;
  Instruction[*] instructions;
}

macro Opcode get_opcode(uint num) {
  return (Opcode)((num >> 26) & 0x3F);
}

macro uint op_first(uint num) {
  return 0; //TODO:
}

macro uint op_second(uint num) {
  return 0; //TODO:
}

macro uint op_third(uint num) {
  return 0; //TODO:
}

fn void execute_instructions(VmContext* ctx) {
  uint instruction = ctx.instructions[ctx.pc];
  switch (get_opcode(instruction)) {
    case Opcode.LOAD: {
      ctx.vm.general_regs[op_first(instruction)] = ctx.vm.ctable[op_second(instruction)].u32;
      break;
    }
    case Opcode.MOV: {
      ctx.vm.general_regs[op_first(instruction)] = ctx.vm.general_regs[op_second(instruction)];
      break;
    }
    case Opcode.ADD: {
      //TODO: add intelligent recognition of register type
      ctx.vm.general_regs[op_third(instruction)] =
        ctx.vm.general_regs[op_first(instruction)] +
        ctx.vm.general_regs[op_second(instruction)];
      break;
    }
    case Opcode.SUB: {
      ctx.vm.general_regs[op_third(instruction)] =
        ctx.vm.general_regs[op_first(instruction)] -
        ctx.vm.general_regs[op_second(instruction)];
      break;
    }
    case Opcode.PUSH: {
      unimplemented("unimplemented push");
      break;
    }
    case Opcode.POP: {
      unimplemented("unimplemented pop");
      break;
    }
    default:
  }
  ctx.pc++;
}

macro unimplemented(String s = "") {
  io::printf("bytecode interpreter unimplemented %s\n");
  libc::exit(1);
}
