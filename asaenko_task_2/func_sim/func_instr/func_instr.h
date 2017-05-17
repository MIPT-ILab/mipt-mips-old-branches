
#include <iostream>
#include <elf_parser.h>
#include <stdlib.h>
#include <types.h>
#include <func_memory.h>

class FuncInstr
{
  public:
    union Parsed
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
            unsigned d:5;
            unsigned t:5;
            unsigned s:5;
            unsigned func:6;
            unsigned opcode:6;
        } asR;
        struct
        {
            unsigned address:26;
            unsigned opcode:6;
        } asJ;
        uint32 raw;
    } bytes;
    
    void initFormat( uint32 bytes);
    void parseR(uint32 bytes);
    void parseI(uint32 bytes);
    void parseJ(uint32 bytes);
    uint8 getISAFromTable(const uint8 opcode, const uint8 func = 0x0) const;
    const char *getISAname(const uint8 id = 0);
    const string getREGname(const uint8 reg = 0);

    uint8 dReg;
    uint8 tReg;
    uint8 sReg;
    uint8 imm;
    uint8 addr;


    FuncInstr( uint32 bytes);
    std::string Dump( std::string indent = " ") const;
    int initFormat( uint32 bytes);
    
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

    FuncInstr::FuncInstr( uint32 bytes)
    std::string Dump( std::string indent = "") const;

    
  };

std::ostream& operator<<(std::ostream& out, const FuncInstr& instr);
