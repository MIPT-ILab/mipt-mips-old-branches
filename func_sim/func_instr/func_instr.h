#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

#include <types.h>
#include <elf_parser.h>

class FuncInstr
{
    union Command
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
            unsigned funct:6;
            unsigned S:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asR;
        struct
        {
            unsigned addres:26;
            unsigned opcode:6;
        } asJ;
        uint32 raw;
    };

    Command bytes;

    void initFormat( uint32);
    void parseR(uint32 bytes);
    void parseI(uint32 bytes);
    void parseJ(uint32 bytes);
    uint8 getISAFromTable(const uint8 opcode, const uint8 func = 0x0) const;
    const char *getISAname(const uint8 id = 0) const;
    const string getREGname(const uint8 reg = 0) const;

    uint8 dReg;
    uint8 tReg;
    uint8 sReg;
    uint8 imm;

public:
    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    } format;

    enum Type
    {
        ADD,
        SUB,
        SHIFT,
        BRANCH,
        JUMP
    };

    FuncInstr( uint32 bytes);
    std::string Dump( std::string indent = "") const;
};

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H