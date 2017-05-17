#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H


#include <cstring>
#include <cassert>
#include <string.h>
#include <sstream>
#include <types.h>
#include <elf_parser.h>
#include <func_memory.h>
#include <errno.h>
#include <stdlib.h>

const uint32 numCom = 12;
class FuncInstr
{
    union
    {
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;
        struct
        {
            unsigned opcode:6;
            unsigned s:5;
            unsigned t:5;
            unsigned d:5;
            unsigned shamt:5;
            unsigned funct:6;
        } asR;
        struct
        {
            unsigned opcode:6;
            unsigned imm:26;
        } asJ;
        uint32 raw;
    } bytes;
    
    unsigned sReg:5;
    unsigned tReg:5;
    unsigned dRed:5;
    uint32 format;
    uint32 type;

    public:
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
        
        struct ISAEntry
        {
            const char* name;

            uint8 opcode;
            uint8 funct;

            uint32 format;
            uint32 type;
        };

        struct REGEntry
        {
            const char* name;
            uint8 addr;
        };

        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J
        };

        enum Type
        {
            ADD,
            ADDU,
            SUB,
            SUBU,
            ADDI,
            ADDIU,
            SLL,
            SRL,
            BEQ,
            BNE,
            JUMP,
            JUMPR
        };

        static const ISAEntry isaTable[ numCom];
        static const REGEntry regTable[ 32];
        void initFormat( uint32);
        void parseR( uint32);
        void parseI( uint32);
        void parseJ( uint32);
};

std::ostream& operator<<(std::ostream& out, const FuncInstr& instr);
#endif
