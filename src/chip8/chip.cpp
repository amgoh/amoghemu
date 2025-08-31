#include "chip.h"
#include <utility>

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

void ChipCPU::Opcode00E0(){
  for(int i = 0; i < SCREEN_WIDTH; i++) {
    for(int j = 0; j < SCREEN_HEIGHT; j++) {
      m_Display[i][j][0] = 255;
      m_Display[i][j][1] = 255;
      m_Display[i][j][2] = 255;
    }
  }
}
void ChipCPU::Opcode0NNN(WORD opcode) {
  // not necessary
}
void ChipCPU::Opcode00EE() {
  m_ProgramCounter = m_Stack.back();
  m_Stack.pop_back();
}
void ChipCPU::Opcode8XY0(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>=4;
  m_Registers[reg_x] = m_Registers[reg_y];
}
void ChipCPU::Opcode8XY1(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8 bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4; // bit-shift right by 4 bits, same as x
  m_Registers[reg_x] |= m_Registers[reg_y];
}
void ChipCPU::Opcode8XY2(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;
  m_Registers[reg_x] &= m_Registers[reg_y];
}
void ChipCPU::Opcode8XY3(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;
  m_Registers[reg_x] ^= m_Registers[reg_y];
}
void ChipCPU::Opcode8XY4(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;
  
  if(m_Registers[reg_x] > 255) m_Registers[0xF] = 1;
  else m_Registers[0xF] = 0;
  
  m_Registers[reg_x] += m_Registers[reg_y];
}
void ChipCPU::Opcode8XY5(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;

  if(m_Registers[reg_x] < m_Registers[reg_y]) m_Registers[0xF] = 0;
  else m_Registers[0xF] = 1;

  m_Registers[reg_x] -= m_Registers[reg_y];
}
void ChipCPU::Opcode8XY6(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  m_Registers[0xF] = m_Registers[reg_x] & 0x1;
  m_Registers[reg_x] >>= 1;
}
void ChipCPU::Opcode8XY7(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>=4;
  if(m_Registers[reg_y] < m_Registers[reg_x]) m_Registers[0xF] = 0;
  else m_Registers[0xF] = 1;

  m_Registers[reg_x] = m_Registers[reg_y] - m_Registers[reg_x];
}
void ChipCPU::Opcode8XYE(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;

  if((m_Registers[reg_x] & 0x1) == 0) m_Registers[0xF] = 0;
  else m_Registers[0xF] = 1;

  m_Registers[reg_x] <<= 1;
}
void ChipCPU::OpcodeEX9E(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  int key = m_Registers[reg_x];

  if(m_KeyInput[key] == 1) m_ProgramCounter += 2;
}
void ChipCPU::OpcodeEXA1(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  int key = m_Registers[reg_x];

  if(m_KeyInput[key] == 0) m_ProgramCounter += 2;
}
void ChipCPU::OpcodeFX07(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  m_Registers[reg_x] = m_DelayTimer;
}
void ChipCPU::OpcodeFX0A(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  int keypressed = -1;
  for(int i = 0; i < 16; i++) {
    if(m_KeyInput[i] > 0) {
      keypressed = i;
      break;
    }
  }

  if(keypressed == -1) m_ProgramCounter -= 2;
  else m_Registers[reg_x] = keypressed;
}
void ChipCPU::OpcodeFX15(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  m_DelayTimer = m_Registers[reg_x];
}
void ChipCPU::OpcodeFX18(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF

  m_SoundTimer = m_Registers[reg_x];
}
void ChipCPU::OpcodeFX1E(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  m_AddressI += m_Registers[reg_x];
}
void ChipCPU::OpcodeFX29(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  m_AddressI = m_Registers[reg_x]*5;
}
void ChipCPU::OpcodeFX33(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  m_Memory[m_AddressI] = m_Registers[reg_x] / 100; // hundreds place
  m_Memory[m_AddressI+1] = (m_Registers[reg_x] / 10) % 10; // tens place
  m_Memory[m_AddressI+2] = m_Registers[reg_x] % 10; // ones place
}
void ChipCPU::OpcodeFX55(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF

  for(int i = 0; i < reg_x; i++) {
    m_Memory[m_AddressI + i] = m_Registers[i];
  }

  // m_AddressI += reg_x + 1;
}
void ChipCPU::OpcodeFX65(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF

  for(int i = 0; i < reg_x; i++) {
    m_Registers[i] = m_Memory[m_AddressI + i];
  }

  //m_AddressI += reg_x + 1;
}
void ChipCPU::Opcode1NNN(WORD opcode) {
  m_ProgramCounter = opcode & 0xFFF;
}
void ChipCPU::Opcode2NNN(WORD opcode) {
  m_Stack.push_back(m_ProgramCounter);
  m_ProgramCounter = opcode & 0xFFF;
}
void ChipCPU::Opcode3XNN(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF

  int next_instruction = opcode & 0xFF;

  if(m_Registers[reg_x] == next_instruction) {
    m_ProgramCounter += 2; 
  }
}
void ChipCPU::Opcode4XNN(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF

  int next_instruction = opcode & 0xFF;

  if(m_Registers[reg_x] != next_instruction) {
    m_ProgramCounter += 2;
  }
}
void ChipCPU::Opcode5XY0(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;

  if(m_Registers[reg_x] == m_Registers[reg_y]) { 
    m_ProgramCounter += 2;
  }
}
void ChipCPU::Opcode6XNN(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  int nn = opcode & 0xFF;

  m_Registers[reg_x] = nn;
}
void ChipCPU::Opcode7XNN(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  
  int nn = opcode & 0xFF;

  m_Registers[reg_x] += nn;
}
void ChipCPU::Opcode9XY0(WORD opcode) {
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;

  if(m_Registers[reg_x] != m_Registers[reg_y]) {
    m_ProgramCounter += 2;
  }
}
void ChipCPU::OpcodeANNN(WORD opcode) {
  m_AddressI = opcode & 0xFFF;
}
void ChipCPU::OpcodeBNNN(WORD opcode) {
  m_ProgramCounter = m_Registers[0] + (opcode & 0xFFF);
}
void ChipCPU::OpcodeCXNN(WORD opcode){
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF

  m_Registers[reg_x] = rand() & (opcode & 0xFF);
}
void ChipCPU::OpcodeDXYN(WORD opcode){
  int reg_x = opcode & 0x0F00;
  reg_x >>= 8; // bit-shift right by 8-bits, 0x0F00 >> 8 == 0xF
  int reg_y = opcode & 0x00F0;
  reg_y >>= 4;

  const int SCALE = SCREEN_WIDTH / 64;
  int coord_x = m_Registers[reg_x];
  int coord_y = m_Registers[reg_y];
  int height = opcode & 0xF;

  m_Registers[0xF] = 0;
  // start top left
  for(int row = 0; row < 8; row++) {
    BYTE row_data = m_Memory[m_AddressI + row]; // data of each row, offset by y

    // used for masking
    int col = 0;
    int col_inverse = 7;

    for(col = 0; col < height; col++, col_inverse--) {
      int pixel_mask = 1 << col_inverse; // mask to check if current column bit is set
      if(row_data & pixel_mask) { // bit is set 
        int x = (col * SCALE) + coord_x; // x pixel
        int y = (row * SCALE) + coord_y; // y pixel

        int color = 0;

        if(m_Display[x][y][0] == 0) {
          color = 255;
          m_Registers[0xF] = 1;
        }

        for(int xi = 0; xi < SCALE; xi++) {
          for(int yi = 0; yi < SCALE; yi++) {
            m_Display[x+xi][y+yi][0] = color;
            m_Display[x+xi][y+yi][1] = color;
            m_Display[x+xi][y+yi][2] = color;
          }
        }
      }
    } 
  }
}
