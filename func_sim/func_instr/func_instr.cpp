/**
 * func_instr.cpp - The methods for implementing the concept of
 * disassembler of MIPS ISA.
 * @author Mikhail Lyubogoschev <lyubogoshchev@phystech.edu>
 * Copyright 2014 uArchSim iLab project
 */

#include <func_instr.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    //name  opcode func  format               type
    { "add ", 0x0, 0x20, FuncInstr::FORMAT_R, FuncInstr::ADD },
    { "addu ", 0x0, 0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU },
    { "sub ", 0x0, 0x22, FuncInstr::FORMAT_R, FuncInstr::SUB },
    { "subu ", 0x0, 0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU },
    { "addi ", 0x8, 0, FuncInstr::FORMAT_I, FuncInstr::ADDI },
    { "addiu ", 0x9, 0, FuncInstr::FORMAT_I, FuncInstr::ADDIU },
    { "sll ", 0x0, 0x0, FuncInstr::FORMAT_R, FuncInstr::SLL },
    { "srl ", 0x0, 0x2, FuncInstr::FORMAT_R, FuncInstr::SRL },
    { "beq ", 0x4, 0, FuncInstr::FORMAT_I, FuncInstr::BEQ },
    { "bne ", 0x5, 0, FuncInstr::FORMAT_I, FuncInstr::BNE },
    { "j ", 0x2, 0, FuncInstr::FORMAT_J, FuncInstr::J },
    { "jr ", 0x0, 0x8, FuncInstr::FORMAT_R, FuncInstr::JR }
};
const FuncInstr::Reg FuncInstr::regTable[] =
{
    { "$zero", 0 },
    { "$at", 1 },
    { "$v0", 2},
    { "$v1", 3},
    { "$a0", 4},
    { "$a1", 5},
    { "$a2", 6},
    { "$a3", 7},
    { "$t0", 8},
    { "$t1", 9},
    { "$t2", 10},
    { "$t3", 11},
    { "$t4", 12},
    { "$t5", 13},
    { "$t6", 14},
    { "$t7", 15},
    { "$s0", 16},
    { "$s1", 17},
    { "$s2", 18},
    { "$s3", 19},
    { "$s4", 20},
    { "$s5", 21},
    { "$s6", 22},
    { "$s7", 23},
    { "$t8", 24},
    { "$t9", 25},
    { "$k0", 26},
    { "$k1", 27},
    { "$gp", 28},
    { "$sp", 29},
    { "$fp", 30},
    { "$ra", 31}
};

FuncInstr::FuncInstr( uint32 bytes):
    opcMask( 0xff << 26),
    funcMask( 0x3f)
{
    if ( bytes == ~0ull)
        exit( EXIT_FAILURE);
    dumpstr << hex << showbase;
    this->InitFormat( bytes);
    switch ( format)
    {
        case FORMAT_R:
            ParseR();
            break;
        case FORMAT_I:
            ParseI();
            break;
        case FORMAT_J:
            ParseJ();
            break;
        default:
            assert( 0);
    }
}

FuncInstr:: ~FuncInstr(){}


void FuncInstr::InitFormat( uint32 _bytes)
{
    this->bytes.raw = _bytes;
    switch ( this->bytes.asR.opcode)
    {
        case 0x0:
            format = FORMAT_R;
            break;
        case 0x2:
            format = FORMAT_J;
            break;
        default:
            format = FORMAT_I;
            break;
    }
}

void FuncInstr::ParseR()
{
    reg1 =  regTable[bytes.asR.d].name;
    reg2 = regTable[bytes.asR.s1].name;
    reg3 = regTable[bytes.asR.t].name;
    switch ( bytes.asR.funct)
    {
        case 0x20:
            name = isaTable[ADD].name;
            dumpstr << name << reg1 << ", " << reg2 << ", " << reg3;
            break;
        case 0x21:
            name = isaTable[ADDU].name;
            dumpstr << name << reg1 << ", " << reg2 << ", " << reg3;
            break;
        case 0x22:
            name = isaTable[SUB].name;
            dumpstr << name << reg1 << ", " << reg2 << ", " << reg3;
            break;
        case 0x23:
            name = isaTable[SUBU].name;
            dumpstr << name << reg1 << ", " << reg2 << ", " << reg3;
            break;
        case 0x0:
            name = isaTable[SLL].name;
            cnst = bytes.asR.s2;
            dumpstr << name << reg1 << ", " << reg3 << ", " << cnst;
            break;
        case 0x2:
            name = isaTable[SRL].name;
            cnst = bytes.asR.s2;
            dumpstr << name << reg1 << ", " << reg3 << ", " << cnst;
            break;
        default:
            cerr << "ERROR: wrong command R\n";
            exit( EXIT_FAILURE);
    }
}

void FuncInstr::ParseI()
{
    reg1 = regTable[bytes.asI.t].name;
    reg2 = regTable[bytes.asI.s].name;
    cnst = bytes.asI.imm;
    switch ( bytes.asI.opcode )
    {
        case 0x8:
            name = isaTable[ADDI].name;
            dumpstr << name << reg1 << ", " << reg2 << ", " << cnst;
            break;
        case 0x9:
            name = isaTable[ADDIU].name;
            dumpstr << name << reg1 << ", " << reg2 << ", " << cnst;
            break;
        case 0x4:
            name = isaTable[BEQ].name;
            dumpstr << name << reg1 << ", " << reg2 << ", " << cnst;
            break;
        default:
            cerr << "ERROR: wrong command I\n";
            exit( EXIT_FAILURE);
    }
}

void FuncInstr::ParseJ()
{
    switch ( bytes.asJ.opcode )
    {
        case 0x2:
            name = isaTable[J].name;
            cnst = bytes.asJ.addr;
            dumpstr << name << cnst;
            break;
        default:
            cerr << "ERROR: wrong command J\n";
            exit( EXIT_FAILURE);
    }
}

inline std::string FuncInstr::Dump( std::string indent) const
{
    return indent + dumpstr.str();
}

std::ostream& operator << ( std::ostream& out, FuncInstr& instr)
{
    return out << instr.Dump("");
}
