#include "chip.h"

void ChipCPU::CPUReset() {
  m_AddressI = 0;
  m_ProgramCounter = 0x200;
  memset(m_Registers, 0, sizeof(m_Registers));
  
  FILE *in;
  in = std::fopen("../../ROMS/INVADERS", "rb");

  if (in == 0) {
    printf("ROM does not exist!");
    return;
  }

  std::fread(&m_Memory[0x200], 0xFFF, 1, in);
  fclose(in);
}

WORD ChipCPU::NextOpcode() {
  WORD opc = 0;
  opc = m_Memory[m_ProgramCounter];
  opc <<= 8;
  opc |= m_Memory[m_ProgramCounter+1];
  m_ProgramCounter += 2;
  return opc;
}

void ChipCPU::DecodeOpcode() {
  WORD opcode = NextOpcode();
  switch (opcode & 0xF000) {
    case 0x0: // 0 leading opcodes
      if((opcode & 0xF0) == 0xE0) { Opcode00E0(); return; }
      
      switch (opcode & 0xF) {
        case 0x0: Opcode0NNN(opcode); break;
        case 0xE: Opcode00EE(); break;
        default: break;
      }
    case 0x8000: // 8 leading opcodes
      switch(opcode & 0xF) {
        case 0x0: Opcode8XY0(opcode); break;
        case 0x1: Opcode8XY1(opcode); break;
        case 0x2: Opcode8XY2(opcode); break;
        case 0x3: Opcode8XY3(opcode); break;
        case 0x4: Opcode8XY4(opcode); break;
        case 0x5: Opcode8XY5(opcode); break;
        case 0x6: Opcode8XY6(opcode); break;
        case 0x7: Opcode8XY7(opcode); break;
        case 0xE: Opcode8XYE(opcode); break;
        default: break;
      }
    case 0xE000: // E opcodes
      switch(opcode & 0xF) {
        case 0xE:  OpcodeEX9E(opcode); break;
        case 0x1:  OpcodeEXA1(opcode); break;
        default: break;
      }
    case 0xF000: // F opcodes
      switch(opcode & 0xFF) {
        case 0x07: OpcodeFX07(opcode); break;  
        case 0x0A: OpcodeFX0A(opcode); break;
        case 0x15: OpcodeFX15(opcode); break;
        case 0x18: OpcodeFX18(opcode); break;
        case 0x1E: OpcodeFX1E(opcode); break;
        case 0x29: OpcodeFX29(opcode); break;
        case 0x33: OpcodeFX33(opcode); break;
        case 0x55: OpcodeFX55(opcode); break;
        case 0x65: OpcodeFX65(opcode); break;
        default: break;
      }
    // Remaining Miscellaneous Opcodes
    case 0x1000: Opcode1NNN(opcode); break;
    case 0x2000: Opcode2NNN(opcode); break;
    case 0x3000: Opcode3XNN(opcode); break;
    case 0x4000: Opcode4XNN(opcode); break;
    case 0x5000: Opcode5XY0(opcode); break;
    case 0x6000: Opcode6XNN(opcode); break;
    case 0x7000: Opcode7XNN(opcode); break;
    case 0x9000: Opcode9XY0(opcode); break;
    case 0xA000: OpcodeANNN(opcode); break;
    case 0xB000: OpcodeBNNN(opcode); break;
    case 0xC000: OpcodeCXNN(opcode); break;
    case 0xD000: OpcodeDXYN(opcode); break;
    default: break;
  }
}
