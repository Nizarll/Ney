module bytecode;

import std::libc;
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

macro GetOpCode(uint instruction) {
  return (num >> 26) & 0x3F;
}

def ExecuteInstruction(VmContext* ctx) {
  uint instruction = ctx.program[ctx.pc];
  switch (GETOPCODE(instruction)) {
    case Opcode.LOAD: {
      ctx.vm.general_regs[instruction.operand1] = ctx.vm.ctable[instruction.operand2].u32;
      break;
    }
    case Opcode.MOV: {
      ctx.vm.general_regs[instruction.operand2] = ctx.vm.general_regs[instruction.operand1];
      break;
    }
    case Opcode.ADD: {
      //TODO: add intelligent recognition of register type
      ctx.vm.general_regs[instruction.operand3] =
        ctx.vm.general_regs[instruction.operand1] +
        ctx.vm.general_regs[instruction.operand2];
      break;
    }
    case Opcode.SUB: {
      ctx.vm.general_regs[instruction.operand3] =
        ctx.vm.general_regs[instruction.operand1] -
        ctx.vm.general_regs[instruction.operand2];
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
