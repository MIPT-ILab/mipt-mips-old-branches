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
    { "lui",    0xF, 0x0,   FORMAT_I, OUT_I_SHAMT},
    { "and",    0x0, 0x24,  FORMAT_R, OUT_R_LOGICAL},
    { "or",     0x0, 0x25,  FORMAT_R, OUT_R_LOGICAL},
    { "xor",    0x0, 0x26,  FORMAT_R, OUT_R_LOGICAL},
    { "andi",   0xC, 0x0,   FORMAT_I, OUT_I_LOGICAL},
    { "ori",    0xD, 0x0,   FORMAT_I, OUT_I_LOGICAL},
    { "xori",   0xE, 0x0,   FORMAT_I, OUT_I_LOGICAL},
    { "lb",     0x20, 0x0,  FORMAT_I, OUT_I_LOAD},
    { "lh",     0x21, 0x0,  FORMAT_I, OUT_I_LOAD},
    { "lw",     0x23, 0x0,  FORMAT_I, OUT_I_LOAD},
    { "lbu",    0x24, 0x0,  FORMAT_I, OUT_I_LOAD},
    { "lhu",    0x25, 0x0,  FORMAT_I, OUT_I_LOAD},
    { "sb",     0x28, 0x0,  FORMAT_I, OUT_I_STORE},
    { "sh",     0x29, 0x0,  FORMAT_I, OUT_I_STORE},
    { "sw",     0x2b, 0x0,  FORMAT_I, OUT_I_STORE}
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

FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr( bytes), PC(PC)
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
            oss << regTable[instr.asR.rd] << ", $" 
                << regTable[instr.asR.rs] << ", $" 
                << regTable[instr.asR.rt];
            break;
        case OUT_R_SHAMT:
            oss << regTable[instr.asR.rd] << ", $" 
                << regTable[instr.asR.rt] << ", " 
                << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << regTable[instr.asR.rs];
            break;
        case OUT_R_LOGICAL:
            oss << regTable[instr.asR.rd] <<", $"
                << regTable[instr.asR.rt] <<", $"
                << regTable[instr.asR.rs];
            break;
    }
    disasm = oss.str();
}


void FuncInstr::initI()
{
    std::ostringstream oss;
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            oss << regTable[instr.asI.rt] << ", $"
                << regTable[instr.asI.rs] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_BRANCH:
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_LOAD:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec << "($"
                << regTable[instr.asI,rs] << ")";
            break;
        case OUT_I_STORE:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec << "($"
                << regTable[instr.asI,rs] << ")";
            break;
        case OUT_I_LOGICAL:
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
    }
    disasm = oss.str();
}

void FuncInstr::initJ()
{
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
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

void FuncInstr::add() { v_dst = v_src1 + v_src2; }

void FuncInstr::addu() { v_dst = v_src1 + v_src2; }

void FuncInstr::sub() { v_dst = v_src2 - v_src1; }

void FuncInstr::subu() { v_dst = v_src2 - v_src1; }

void FuncInstr::addi() { v_dst = v_src1 + instr.asI.imm; }

void FuncInstr::addiu() { v_dst = v_src1 + instr.asI.imm; }

void FuncInstr::sll() { v_dst = v_src1 << instr.asR.shamt; }

void FuncInstr::srl() { v_dst = v_src1 >> instr.asR.shamt; }

void FuncInstr::lui() { v_dst = instr.asI.imm << 16; }

void FuncInstr::_and() { v_dst = v_src1 & v_src2; }

void FuncInstr::_or() { v_dst = v_src1 | v_src2; }

void FuncInstr::_xor() { v_dst = v_src1 ^ v_src2; }

void FuncInstr::andi() { v_dst = v_src1 & instr.asI.imm; }

void FuncInstr::ori() { v_dst = v_src1 | instr.asI.imm; }

void FuncInstr::xori() { v_dst = v_src1 ^ instr.asI.imm; }

void FuncInstr::beq() 
{
    if ( v_src1 == v_src2)
        PC = PC + ( instr.asI.imm << 2);
}

void FuncInstr::bne() 
{
    if ( v_src1 != v_src2)
        PC = PC + ( instr.asI.imm << 2);
}

void FuncInstr::j() 
{
    PC = 0;
    jaddr = ( instr.asJ.imm << 2);
}

void FuncInstr::jr() 
{
    PC = 0;
    jaddr = 0;
}

void FuncInstr::lb() 
{   
    bytes = 1;
    ld = true;
    mem_addr = v_src1 + instr.asI.imm;
}

void FuncInstr::lh() 
{
    bytes = 2;
    ld = true;
    mem_addr = v_src1 + instr.asI.imm;
}

void FuncInstr::lw() 
{
    bytes = 4;
    ld = true;
    mem_addr = v_src1 + instr.asI.imm;
}

void FuncInstr::lbu() {
    bytes = 1;
    ld = true;
    mem_addr = v_src1 + instr.asI.imm;
}

void FuncInstr::lhu() {
    bytes = 2;
    ld = true;
    mem_addr = v_src1 + instr.asI.imm;
}

void FuncInstr::sb() {
    bytes = 1;
    ld = false;
    mem_addr = v_src1 + instr.asI.imm;
    v_dst = v_src1;
}

void FuncInstr::sh()
{
    bytes = 2;
    ld = false;
    mem_addr = v_src1 + instr.asI.imm;
    v_dst = v_src1;
}

void FuncInstr::sw()
{
    bytes = 4;
    ld = false;
    mem_addr = v_src1 + instr.asI.imm;
    v_dst = v_src1;
}

int get_src2_num_index() const
{
    switch ( operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_SHAMT:
        case OUT_R_LOGICAL:
        case OUT_I_BRANCH:
        return RegNum(instr.asR.rt);
        break;
        default:
        return RegNum(MAX_REG)
        break;
    }
}

int get_src1_num_index() const
{
    switch ( operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_SHAMT:
        case OUT_R_LOGICAL:
        case OUT_I_ARITHM:
        case OUT_I_LOGICAL:
        case OUT_I_LOAD:
        case OUT_I_STORE:
        case OUT_I_BRANCH
            return RegNum(instr.asR.rs);
        break;
        
        default:
            return RegNum(MAX_REG)
        break;
    }
}

int get_dst_num_index() const
{
    switch ( operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_SHAMT:
        case OUT_R_LOGICAL
            return RegNum(instr.asR.rd)
        break;
        
        case OUT_I_ARITHM:
        case OUT_I_SHAMT:
        case OUT_I_LOGICAL:
            return RegNum(instr.asI.rt)
        break;
        
        case OUT_I_STORE:
            return RegNum(instr.asI.rt)
        break;

        default;
            return RegNum (MAX_REG)
        break;
    }
}