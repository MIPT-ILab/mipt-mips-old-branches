 /**
 * created by Alexander Pronin <alexander.pronin.mipt@gmail.com>
 *
 * v. 1.0: created 05.12.2014 22:06
 *         first version of MIPS disasm
 *
 */

// Generic C++
#include <cstring>
#include <iostream>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>
#include <func_memory.h>
#include <errno.h>
#include <stdlib.h>

typedef unsigned char FormatType;
typedef unsigned char Type;

static const FormatType FORMAT_R = 0;
static const FormatType FORMAT_I = 1;
static const FormatType FORMAT_J = 2;

static const Type ADD   =  0;
static const Type ADDU  =  1;
static const Type SUB   =  2;
static const Type SUBU  =  3;
static const Type ADDI  =  4;
static const Type ADDIU =  5;
static const Type SLL   =  6;
static const Type SRL   =  7;
static const Type BEQ   =  8;
static const Type BNE   =  9;
static const Type J     = 10;
static const Type JR    = 11;
static const Type lastType = JR;


std::string hexi( unsigned val);

class ISAEntry
{
public:
    const char* name;

    uint8 opcode;
    uint8 func;

    FormatType format;
    Type type;
};

class RegEntry
{
public:
    const char* name;
    
    uint8 addr;
};

class FuncInstr
{
private:
    
    FormatType format;

    Type type;
    
    union
    {
        struct
        {
            unsigned imm : 16;
            unsigned t : 5;
            unsigned s : 5;
            unsigned opcode : 6;
        } asI;
        struct
        {
            unsigned func : 6;
            unsigned shamt : 5;
            unsigned d : 5;
            unsigned t : 5;
            unsigned s : 5;
            unsigned opcode : 6;
        } asR;
        struct
        {
            unsigned addr : 26;
            unsigned opcode : 6;
        } asJ;
        uint32 raw;
    } operation;

public:
    FuncInstr( uint32 bytes);
    std::string Dump( std::string indent = " ") const;
    int initFormat( uint32 bytes);
};

std::ostream& operator<<(std::ostream& out, const FuncInstr& instr);
