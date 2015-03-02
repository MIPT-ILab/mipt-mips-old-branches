/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */


#include <func_instr.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>

// description of all known operations
const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name  opcode  func   format    operation
    { "add",    0x0,  0x20,  FORMAT_R, OUT_R_ARITHM},
    { "addu",   0x0,  0x21,  FORMAT_R, OUT_R_ARITHM},
    { "sub",    0x0,  0x22,  FORMAT_R, OUT_R_ARITHM},
    { "subu",   0x0,  0x23,  FORMAT_R, OUT_R_ARITHM},
    { "addi",   0x8,  0x0,   FORMAT_I, OUT_I_ARITHM},
    { "addiu",  0x9,  0x0,   FORMAT_I, OUT_I_ARITHM},
    { "sll",    0x0,  0x0,   FORMAT_R, OUT_R_SHAMT},
    { "srl",    0x0,  0x2,   FORMAT_R, OUT_R_SHAMT},
    { "lui",    0xF,  0x0,   FORMAT_I, OUT_I_SHIFT},
    { "and",    0x0,  0x24,  FORMAT_R, OUT_R_LOG},
    { "or",     0x0,  0x25,  FORMAT_R, OUT_R_LOG},
    { "xor",    0x0,  0x26,  FORMAT_R, OUT_R_LOG},
    { "andi",   0xC,  0x0,   FORMAT_I, OUT_I_LOG},
    { "ori",    0xD,  0x0,   FORMAT_I, OUT_I_LOG},
    { "xori",   0xE,  0x0,   FORMAT_I, OUT_I_LOG},
    { "beq",    0x4,  0x0,   FORMAT_I, OUT_I_BRANCH},
    { "bne",    0x5,  0x0,   FORMAT_I, OUT_I_BRANCH},
    { "j",      0x2,  0x0,   FORMAT_J, OUT_J_JUMP},
    { "jr",     0x0,  0x8,   FORMAT_R, OUT_R_JUMP},
    { "lb",     0x20, 0x0,   FORMAT_I, OUT_I_MEM},
    { "lw",     0x23, 0x0,   FORMAT_I, OUT_I_MEM},
    { "sb",     0x28, 0x0,   FORMAT_I, OUT_I_MEM},
    { "sw",     0x2B, 0x0,   FORMAT_I, OUT_I_MEM}
};

// number of known operations
const uint32 FuncInstr::isaTableSize = sizeof( isaTable) / sizeof( isaTable[0]);

// list of registers
const char *FuncInstr::regTable[] = 
{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", 
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra"
};

// start work with the operation
FuncInstr::FuncInstr( uint32 bytes) : instr(bytes)
{
    initFormat(); 
    switch ( format)
    {
        case FORMAT_R:
            initR();
            break;
        case FORMAT_I:
            initI();
            break;
        case FORMAT_J:
            initJ();
            break;
        case FORMAT_UNKNOWN:
            initUnknown();
            break;
    }
}

// get dump of operation
std::string FuncInstr::Dump( std::string indent) const
{
    return indent + disasm;
}

// get information about operation format
void FuncInstr::initFormat()
{
    for ( size_t i = 0; i < isaTableSize; i++) {
        if ( instr.asR.opcode == isaTable[ i].opcode)
        {
            format = isaTable[ i].format;
            operation = isaTable[ i].operation;
            isaNum = i;
            return;
        }
    }
    format = FORMAT_UNKNOWN;
}

// execute the operation and prepare its dump
void FuncInstr::initR()
{
    // find instr by functor
    for ( isaNum = 0; isaNum < isaTableSize; ++isaNum) {
        if (( instr.asR.opcode == isaTable[ isaNum].opcode) &&
            ( instr.asR.funct == isaTable[ isaNum].funct))
        {
            operation = isaTable[ isaNum].operation;
            break;
        }
    }
    if ( isaNum == isaTableSize)     // if didn't found funct
    {
        initUnknown();
        return;
    }
    
    ostringstream oss;
    oss << isaTable[ isaNum].name << " $";
    switch ( operation)
    {
        case OUT_R_ARITHM:
            i_dst = instr.asR.rd;
            i_src1 = instr.asR.rs;
            i_src2 = instr.asR.rt;

            MIPS::read_reg( this); // this function just like read_src on mipt-mips wiki, but it also reads the dst value

            oss << regTable[ instr.asR.rd] << outDst() << ", $"
                << regTable[ instr.asR.rs] << outSrc1() << ", $"
                << regTable[ instr.asR.rt] << outSrc2();
            break;

        case OUT_R_SHAMT:
            i_dst = instr.asR.rd;
            i_src1 = instr.asR.rt;
            i_src2 = NULL;

            MIPS::read_reg( this);

            oss << regTable[ instr.asR.rd] << outDst() << ", $"
                << regTable[ instr.asR.rt] << outSrc1() << ", "
                << dec << instr.asR.shamt;
            break;

        case OUT_R_JUMP:
            i_dst = NULL;
            i_src1 = instr.asR.rs;
            i_src2 = NULL;

            MIPS::read_reg( this);

            oss << regTable[ instr.asR.rs] << outSrc1();
            break;

        case OUT_R_LOG:
            i_dst = instr.asR.rd;
            i_src1 = instr.asR.rt;
            i_src2 = instr.asR.rs;

            MIPS::read_reg( this);

            oss << regTable[ instr.asR.rd] << outDst() << ", $"
            oss << regTable[ instr.asR.rt] << outSrc1() << ", $"
            oss << regTable[ instr.asR.rs] << outSrc2();
            break;
    }
    disasm = oss.str();
}

// execute the operation and prepare its dump
void FuncInstr::initI()
{
    std::ostringstream oss;
    oss << isaTable[ isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            i_dst = instr.asI.rt;
            i_src1 = instr.asI.rs;
            i_src2 = NULL;

            MIPS::read_reg( this);
            
            oss << regTable[ instr.asI.rt] << outDst() << ", $"
                << regTable[ instr.asI.rs] << outSrc1() << ", "
                << outImm();
            break;

        case OUT_I_BRANCH:
            i_dst = NULL;
            i_src1 = instr.asI.rs;
            i_src2 = instr.asI.rt;

            MIPT::read_reg( this);

            oss << regTable[ instr.asI.rs] << outSrc1() << ", $"
                << regTable[ instr.asI.rt] << outSrc2() << ", "
                << outImm();
            break;

        case OUT_I_SHIFT:
            i_dst = instr.asI.rt;
            i_src1 = i_src2 = NULL;

            MIPS::read_reg( this);

            oss << regTable[ instr.asI.rt] << outDst() << ", "
                << outImm();
            break;

        case OUT_I_LOGIC:
            i_dst = instr.asI.rt;
            i_src1 = instr.asI.rs;
            i_src2 = NULL;

            MIPS::read_reg( this);

            oss << regTable[ instr.asI.rs] << outSrc1() << ", $"
                << regTable[ instr.asI.rt] << outDst() << ", "
                << outImm();
            break;

        case OUT_I_MEM:
            i_dst = NULL; // if we load data it is set in execute function
            i_src1 = instr.asI.rt;
            i_src2 = instr.asi.rs;

            MIPS::read_reg( this);

            oss << regTable[ instr.asI.rt] << outSrc1 << ", "
                << outImm() << "($" << regTable[ instr.asI.rs] << ")";
            break;

    }
    disasm = oss.str();
}

// execute the operation and prepare its dump
void FuncInstr::initJ()
{
    std::ostringstream oss;

    oss << isaTable[ isaNum].name << " "
        << std::hex << "0x" << instr.asJ.imm;
    disasm = oss.str();
}

// give the message about error if the operation is unknown
void FuncInstr::initUnknown()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown" << std::endl;
    disasm = oss.str();
    std::cerr << "ERROR. Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

// usefull thing for easy dump
std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

std::string FuncInstr::outImm()
{
    std::ostringstream oss;
    oss << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
    return oss.str();
}

std::string FuncInstr::outSrc1()
{
    std::ostringstream oss;
    oss << " [" << std::hex << "0x" << static_cast< signed int>( v_src1) << std::dec << "]";
    return oss.str();
}

std::string FuncInstr::outSrc2()
{
    std::ostringstream oss;
    oss << " [" << std::hex << "0x" << static_cast< signed int>( v_src2) << std::dec << "]";
    return oss.str();
}

std::string FuncInstr::outDst()
{
    std::ostringstream oss;
    oss << " [" << std::hex << "0x" << static_cast< signed int>( v_Dst) << std::dec << "]";
    return oss.str();
}