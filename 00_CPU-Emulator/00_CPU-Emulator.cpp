// 00_CPU-Emulator.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <vector>
#include <algorithm>

using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int; 

struct Memory
{
    static constexpr u32 MAX_MEMORY = 1024 * 64; 
    Byte data[MAX_MEMORY];

    void Init()
    {
        for (u32 i = 0; i < MAX_MEMORY; i++)
        {
            data[i] = 0; 
        }
    }

    Byte operator[](u32 address) const
    {
        return data[address];
    }

    Byte& operator[](u32 address)
    {
        return data[address];
    }
};

struct CPU_6502
{
    Word PC; // Program Counter
    Word SP; // Stack Pointer

    // registers
    Byte A, X, Y; 

    // Processor status flags
    Byte CARRY : 1; 
    Byte ZERO : 1; 
    Byte INTERRUPT : 1;
    Byte DECIMAL : 1; 
    Byte BREAK : 1; 
    Byte OVFLOW : 1; 
    Byte NEGATIVE : 1; 

    void Reset(Memory &memory)
    {
        PC = 0xFFFC; 
        SP = 0x0100; 
        CARRY = ZERO = INTERRUPT = DECIMAL = BREAK = OVFLOW = NEGATIVE = 0; 
        A = X = Y = 0; 
        memory.Init(); 
    }

    Byte FetchByte(u32 cycles, Memory& memory)
    {
        Byte data = memory[PC];
        PC++; 
        cycles--;
        return data; 
    }

    Byte ReadByte(u32 cycles, Byte address, Memory& memory)
    {
        Byte data = memory[PC];
        cycles--;
        return data;
    }

    // two instructions: IMMEDIATE / ZERO PAGE
    static constexpr Byte INST_LDA_IMD = 0xA9; 
    static constexpr Byte INST_LDA_ZPG = 0xA5;

    void SetLDAStatus()
    {
        ZERO = (A == 0);
        NEGATIVE = (A & 0b10000000) > 0; 
    }

    void Execute(u32 cycles, Memory& memory)
    {
        while (cycles > 0)
        {
            Byte inst = FetchByte(cycles, memory);
            
            switch (inst)
            {
                case INST_LDA_IMD:
                {
                    Byte value = FetchByte(cycles, memory);
                    A = value;
                    SetLDAStatus(); 
                }
                case INST_LDA_ZPG:
                {
                    Byte zeropage_addr = FetchByte(cycles, memory);
                    A = ReadByte(cycles, zeropage_addr, memory);
                    SetLDAStatus(); 
                }
                default:
                {
                    std::cout << "[EM6502_SHELL_MSG] INSTRUCTION [" << inst << "] NOT HANDLED" << std::endl;
                }
                break;
            }
        }
    }
};

int main()
{
    // init CPU and Memory
    CPU_6502 cpu; 
    Memory memory;

    // perform a ZERO PAGE instruction
    cpu.Reset(memory);
    memory[0xFFFC] = CPU_6502::INST_LDA_ZPG;
    memory[0xFFFD] = 0x42;
    memory[0x0042] = 0x84; 
    cpu.Execute(3, memory);

    // perform a IMMEDIATE instruction
    cpu.Reset(memory);
    memory[0xFFFC] = CPU_6502::INST_LDA_IMD;
    memory[0xFFFD] = 0x42;
    cpu.Execute(2, memory);
    
    return 0; 
}
