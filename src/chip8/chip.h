#pragma once
#ifndef _CHIP_H
#define _CHIP_H

#include <string>
#include <vector>

typedef unsigned char BYTE;
typedef unsigned short WORD;

const int ROM_SIZE = 0xFFF;

class ChipCPU {
  public:
    ~ChipCPU();
    static ChipCPU* Singleton();

    BYTE m_Display[640][320];
  private:
    ChipCPU();

    void CPUReset();

    WORD NextOpcode();
    void DecodeOpcode();

    void Opcode0NNN(WORD opcode);
    void Opcode00E0();
    void Opcode00EE();
    void Opcode1NNN(WORD opcode);
    void Opcode2NNN(WORD opcode);
    void Opcode3XNN(WORD opcode);
    void Opcode4XNN(WORD opcode);
    void Opcode5XY0(WORD opcode);
    void Opcode6XNN(WORD opcode);
    void Opcode7XNN(WORD opcode);
    void Opcode8XY0(WORD opcode);
    void Opcode8XY1(WORD opcode);
    void Opcode8XY2(WORD opcode);
    void Opcode8XY3(WORD opcode);
    void Opcode8XY4(WORD opcode);
    void Opcode8XY5(WORD opcode);
    void Opcode8XY6(WORD opcode);
    void Opcode8XY7(WORD opcode);
    void Opcode8XYE(WORD opcode);
    void Opcode9XY0(WORD opcode);
    void OpcodeANNN(WORD opcode);
    void OpcodeBNNN(WORD opcode);
    void OpcodeCXNN(WORD opcode);
    void OpcodeDXYN(WORD opcode);
    void OpcodeEX9E(WORD opcode);
    void OpcodeEXA1(WORD opcode);
    void OpcodeFX07(WORD opcode);
    void OpcodeFX0A(WORD opcode);
    void OpcodeFX15(WORD opcode);
    void OpcodeFX18(WORD opcode);
    void OpcodeFX1E(WORD opcode);
    void OpcodeFX29(WORD opcode);
    void OpcodeFX33(WORD opcode);
    void OpcodeFX55(WORD opcode);
    void OpcodeFX65(WORD opcode);

    BYTE m_Memory[ROM_SIZE];
    BYTE m_Registers[16];
    WORD m_AddressI;
    WORD m_ProgramCounter;

    std::vector<WORD> m_Stack;
    BYTE m_KeyInput[16];
    BYTE m_DelayTimer;
    BYTE m_SoundTimer;
};

#endif
