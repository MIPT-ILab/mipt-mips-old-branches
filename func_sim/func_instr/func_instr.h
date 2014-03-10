#ifndef FUNC_INSTR_H

#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>
#include <set>
#include <sstream>

// uArchSim modules
#include <types.h>

using namespace std;

class FuncInstr
{
    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    } format;
    
    enum Type
    {
        ADD,
        SHIFT,
        BRANCH,
        JUMP
    };
 
    union
    {
        struct
        {
            unsigned funct:6;
            unsigned shamt:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asR;
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;
        struct
        {
            unsigned addr:26;
            unsigned opcode:6;
        } asJ;
        uint32 raw;
    } bytes;
    
    struct ISAEntry
    {
        const char* name;
        uint8 opcode;
        uint8 funct;
        FuncInstr::Format format;
        FuncInstr::Type type;
    };
    
    struct Instruction
    {
        int8 numb; //Number in isaTable
        uint8 regs;
        uint8 regt;
        uint8 regd;
        uint8 shamt;
        uint16 imm;
        uint32 addr;
    } instr;
    
    static const ISAEntry isaTable[];
//    static const set<unsigned> reg;
    static const string regString[];
    static const int8 tablesize;
public:

    FuncInstr ( uint32 bytes);
    string Dump( string indent = "") const;
    void initFormat( uint32 bytes);
    void parseR();
    void parseI();
    void parseJ();
};

ostream& operator<<( std::ostream& out, const FuncInstr& instr);
#endif
