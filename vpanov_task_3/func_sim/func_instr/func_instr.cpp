/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */


#include <func_instr.h>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include "func_instr.h"

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name  opcode  func   format    operation
    { "add",    0x0, 0x20,  FORMAT_R, OUT_R_ARITHM},
    { "addu",   0x0, 0x21,  FORMAT_R, OUT_R_ARITHM},
    { "sub",    0x0, 0x22,  FORMAT_R, OUT_R_ARITHM},
    { "subu",   0x0, 0x23,  FORMAT_R, OUT_R_ARITHM},
    { "addi",   0x8, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "addiu",  0x9, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "sll",    0x0, 0x0,   FORMAT_R, OUT_R_SHAMT},
    { "srl",    0x0, 0x2,   FORMAT_R, OUT_R_SHAMT},
    { "beq",    0x4, 0x0,   FORMAT_I, OUT_I_BRANCH},
    { "bne",    0x5, 0x0,   FORMAT_I, OUT_I_BRANCH},
    { "j",      0x2, 0x0,   FORMAT_J, OUT_J_JUMP},
    { "jr",     0x0, 0x8,   FORMAT_R, OUT_R_JUMP},
    { "la",     0xf, 0x0,   FORMAT_I, OUT_I_LA},
    { "lb",     0x20,0x0,   FORMAT_I, OUT_I_LB},
    { "lw",     0x23,0x0,   FORMAT_I, OUT_I_LW},
    { "sb",     0x28,0x0,   FORMAT_I, OUT_I_SB},
    { "sw",     0x2B,0x0,   FORMAT_I, OUT_I_SW},
};
const uint32 FuncInstr::isaTableSize = sizeof(isaTable) / sizeof(isaTable[0]);

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

FuncInstr::FuncInstr( uint32 bytes) :
        instr(bytes),
        exec_operation(NULL),
        PC_delta(0),
        mem_addr(0)
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

std::string FuncInstr::Dump( std::string indent) const
{
    return indent + disasm;
}

void FuncInstr::initFormat()
{
    for ( size_t i = 0; i < isaTableSize; i++) {
        if ( instr.asR.opcode == isaTable[i].opcode)
        {
            format = isaTable[i].format;
            operation = isaTable[i].operation;
            isaNum = i;
            return;
        }
    }
    format = FORMAT_UNKNOWN;
}


void FuncInstr::initR()
{
    // find instr by functor
    for (isaNum = 0; isaNum < isaTableSize; ++isaNum) {
        if (( instr.asR.opcode == isaTable[isaNum].opcode) &&
            ( instr.asR.funct == isaTable[isaNum].funct))
        {
            operation = isaTable[isaNum].operation;
            break;
        }
    }
    if ( isaNum == isaTableSize)     // if didn't found funct
    {
        initUnknown();
        return;
    }

    ostringstream oss;
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_R_ARITHM:
            rd = instr.asR.rd;
            rs = instr.asR.rs;
            rt = instr.asR.rt;
            if(isaTable[isaNum].funct < 0x22) {
                exec_operation = &FuncInstr::add;
            }
            else {
                exec_operation = &FuncInstr::sub;
            }
            oss << regTable[instr.asR.rd] << ", $" \
                << regTable[instr.asR.rs] << ", $" \
                << regTable[instr.asR.rt];
            break;
        case OUT_R_SHAMT:
            if(isaTable[isaNum].funct == 0x0) {
                exec_operation = &FuncInstr::sll;
            }
            else {
                exec_operation = &FuncInstr::srl;
            }
            rd = instr.asR.rd;
            rt = instr.asR.rt;
            shamt = instr.asR.shamt;
            oss << regTable[instr.asR.rd] << ", $" \
                << regTable[instr.asR.rt] << ", " \
                << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            exec_operation = &FuncInstr::jr;
            rs = instr.asR.rs;
            oss << regTable[instr.asR.rs];
            break;
    }
    disasm = oss.str();
}


void FuncInstr::initI()
{

    //v_dst = instr.asI.rt;
    //v_src1 = instr.asI.rs;
    //v_src2 = instr.asI.imm;
    std::ostringstream oss;
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            exec_operation = &FuncInstr::add;
            rt  = instr.asI.rt;
            rs  = instr.asI.rs;
            imm = instr.asI.imm;
            oss << regTable[instr.asI.rt] << ", $"
                << regTable[instr.asI.rs] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_BRANCH:
            if(isaTable[isaNum].opcode == 0x4) {
                exec_operation = &FuncInstr::beq;
            }
            else {
                exec_operation = &FuncInstr::bne;
            }
            rs  = instr.asI.rs;
            rt  = instr.asI.rt;
            imm = instr.asI.imm;
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_LA:
            exec_operation = &FuncInstr::la;
            rt  = instr.asI.rt;
            imm = instr.asI.imm;
            oss << regTable[instr.asI.rt] << ","
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_LB:
        case OUT_I_LW:
            rt  = instr.asI.rt;
            rs  = instr.asI.rs;
            imm = instr.asI.imm;
            exec_operation = &FuncInstr::load;
            oss << regTable[instr.asI.rt] << ","
                << static_cast< signed int>( instr.asI.imm)
                << "($" << regTable[instr.asI.rs] << ")";
            break;
        case OUT_I_SB:
        case OUT_I_SW:
            rt  = instr.asI.rt;
            rs  = instr.asI.rs;
            imm = instr.asI.imm;
            exec_operation = &FuncInstr::store;
            oss << regTable[instr.asI.rt] << ","
                << static_cast< signed int>( instr.asI.imm)
                << "($" << regTable[instr.asI.rs] << ")";
            break;
    }
    disasm = oss.str();
}

void FuncInstr::initJ()
{
    //v_src2 = instr.asJ.imm;
    exec_operation = &FuncInstr::j;
    imm = instr.asJ.imm;
    std::ostringstream oss;
    oss << isaTable[isaNum].name << " "
        << std::hex << "0x" <<instr.asJ.imm;
    disasm = oss.str();
}

void FuncInstr::initUnknown()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown" << std::endl;
    disasm = oss.str();
    std::cerr << "opcode:" << std::hex << instr.asI.opcode << std::endl;
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

void FuncInstr::execute() {
    assert(exec_operation != NULL);
    (this->*exec_operation)();
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

