/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 *
 * Modified by Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * 2015 MIPT-MIPS
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
    { "add",    0x0,  0x20,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::add},
    { "addu",   0x0,  0x21,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::addu},
    { "sub",    0x0,  0x22,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::sub},
    { "subu",   0x0,  0x23,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::subu},
    { "addi",   0x8,  0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addi},
    { "addiu",  0x9,  0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addiu},
    { "sll",    0x0,  0x0,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::sll},
    { "srl",    0x0,  0x2,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::srl},
    { "lui",    0xF,  0x0,   FORMAT_I, OUT_I_SHIFT,  &FuncInstr::lui},
    { "and",    0x0,  0x24,  FORMAT_R, OUT_R_LOG,    &FuncInstr::_and},
    { "or",     0x0,  0x25,  FORMAT_R, OUT_R_LOG,    &FuncInstr::_or},
    { "xor",    0x0,  0x26,  FORMAT_R, OUT_R_LOG,    &FuncInstr::_xor},
    { "andi",   0xC,  0x0,   FORMAT_I, OUT_I_LOG,    &FuncInstr::andi},
    { "ori",    0xD,  0x0,   FORMAT_I, OUT_I_LOG,    &FuncInstr::ori},
    { "xori",   0xE,  0x0,   FORMAT_I, OUT_I_LOG,    &FuncInstr::xori},
    { "beq",    0x4,  0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::beq},
    { "bne",    0x5,  0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::bne},
    { "j",      0x2,  0x0,   FORMAT_J, OUT_J_JUMP,   &FuncInstr::j},
    { "jr",     0x0,  0x8,   FORMAT_R, OUT_R_JUMP,   &FuncInstr::jr},
    { "lb",     0x20, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::lb},
    { "lh",     0x21, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::lh},
    { "lw",     0x23, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::lw},
    { "lbu",    0x24, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::lbu},
    { "lhu",    0x25, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::lhu},
    { "sb",     0x28, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::sb},
    { "sh",     0x29, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::sh},
    { "sw",     0x2B, 0x0,   FORMAT_I, OUT_I_MEM,    &FuncInstr::sw}
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
FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr(bytes)
{
    PC_old = PC;
    PC_new = 4 + PC_old;
    memOp = nothing;
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
            function = isaTable[ i].function;
            return;
        }
    }
    format = FORMAT_UNKNOWN;
}

// prepare operation to execute it
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
    

    switch ( operation)
    {
        case OUT_R_ARITHM:
            i_dst = instr.asR.rd;
            i_src1 = instr.asR.rs;
            i_src2 = instr.asR.rt;
            break;

        case OUT_R_SHAMT:
            i_dst = instr.asR.rd;
            i_src1 = instr.asR.rt;
            i_src2 = NULL;
            break;

        case OUT_R_JUMP:
            i_dst = NULL;
            i_src1 = instr.asR.rs;
            i_src2 = NULL;
            break;

        case OUT_R_LOG:
            i_dst = instr.asR.rd;
            i_src1 = instr.asR.rt;
            i_src2 = instr.asR.rs;
            break;
    }
}

// prepare dump string
void FuncInstr::DumpR()
{   
    ostringstream oss;
    oss << isaTable[ isaNum].name << " $";
    switch ( operation)
    {
        case OUT_R_ARITHM:
            oss << regTable[ instr.asR.rd] << outDst() << ", $"
                << regTable[ instr.asR.rs] << outSrc1() << ", $"
                << regTable[ instr.asR.rt] << outSrc2();
            break;

        case OUT_R_SHAMT:
            oss << regTable[ instr.asR.rd] << outDst() << ", $"
                << regTable[ instr.asR.rt] << outSrc1() << ", "
                << dec << instr.asR.shamt;
            break;

        case OUT_R_JUMP:
            oss << regTable[ instr.asR.rs] << outSrc1();
            break;

        case OUT_R_LOG:
            oss << regTable[ instr.asR.rd] << outDst() << ", $"
                << regTable[ instr.asR.rt] << outSrc1() << ", $"
                << regTable[ instr.asR.rs] << outSrc2();
            break;
    }
    disasm = oss.str();
}


// prepare operation to execute it
void FuncInstr::initI()
{
    switch ( operation)
    {
        case OUT_I_ARITHM:
            i_dst = instr.asI.rt;
            i_src1 = instr.asI.rs;
            i_src2 = NULL;
            break;

        case OUT_I_BRANCH:
            i_dst = NULL;
            i_src1 = instr.asI.rs;
            i_src2 = instr.asI.rt;
            break;

        case OUT_I_SHIFT:
            i_dst = instr.asI.rt;
            i_src1 = i_src2 = NULL;
            break;

        case OUT_I_LOG:
            i_dst = instr.asI.rt;
            i_src1 = instr.asI.rs;
            i_src2 = NULL;
            break;

        case OUT_I_MEM:
            i_dst = NULL; // if we load data it is being set in execute function
            i_src1 = instr.asI.rt;
            i_src2 = instr.asI.rs;
            break;

    }
}

// prepare dump string
void FuncInstr::DumpI()
{
    std::ostringstream oss;
    oss << isaTable[ isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            oss << regTable[ instr.asI.rt] << outDst() << ", $"
                << regTable[ instr.asI.rs] << outSrc1() << ", "
                << outImm();
            break;

        case OUT_I_BRANCH:
            oss << regTable[ instr.asI.rs] << outSrc1() << ", $"
                << regTable[ instr.asI.rt] << outSrc2() << ", "
                << outImm();
            break;

        case OUT_I_SHIFT:
            oss << regTable[ instr.asI.rt] << outDst() << ", "
                << outImm();
            break;

        case OUT_I_LOG:
            oss << regTable[ instr.asI.rs] << outSrc1() << ", $"
                << regTable[ instr.asI.rt] << outDst() << ", "
                << outImm();
            break;

        case OUT_I_MEM:
            oss << regTable[ instr.asI.rt] << outSrc1() << ", "
                << outImm() << "($" << regTable[ instr.asI.rs] << ")";
            break;

    }
    disasm = oss.str();
}

// prepare operation to execute it
void FuncInstr::initJ()
{
    
}

// prepare dump string
void FuncInstr::DumpJ()
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

// print imm in hex
std::string FuncInstr::outImm()
{
    std::ostringstream oss;
    oss << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
    return oss.str();
}

// print value of src1 in hex
std::string FuncInstr::outSrc1()
{
    std::ostringstream oss;
    oss << " [" << std::hex << "0x" << static_cast< signed int>( b_src1) << std::dec << "]";
    return oss.str();
}

// print value of src2 in hex
std::string FuncInstr::outSrc2()
{
    std::ostringstream oss;
    oss << " [" << std::hex << "0x" << static_cast< signed int>( b_src2) << std::dec << "]";
    return oss.str();
}

// print value of dst in hex
std::string FuncInstr::outDst()
{
    std::ostringstream oss;
    oss << " [" << std::hex << "0x" << static_cast< signed int>( v_dst) << std::dec << "]";
    return oss.str();
}

// execute the operation
void FuncInstr::execute() {
    ( this->*function)();
    preDump();
}

// operations

void FuncInstr::add() {
    v_dst = v_src1 + v_src2;
}

void FuncInstr::addu() {
    v_dst = v_src1 + v_src2;
}

void FuncInstr::sub() {
    v_dst = v_src2 - v_src1;
}

void FuncInstr::subu() {
    v_dst = v_src2 - v_src1;
}

void FuncInstr::addi() {
    v_dst = v_src1 + instr.asI.imm;
}

void FuncInstr::addiu() {
    v_dst = v_src1 + instr.asI.imm;
}

void FuncInstr::sll() {
    v_dst = v_src1 << instr.asR.shamt;
}

void FuncInstr::srl() {
    v_dst = v_src1 >> instr.asR.shamt;
}

void FuncInstr::lui() {
    v_dst = instr.asI.imm << 16;
}

void FuncInstr::_and() {
    v_dst = v_src1 & v_src2;
}

void FuncInstr::_or() {
    v_dst = v_src1 | v_src2;
}

void FuncInstr::_xor() {
    v_dst = v_src1 ^ v_src2;
}

void FuncInstr::andi() {
    v_dst = v_src1 & instr.asI.imm;
}

void FuncInstr::ori() {
    v_dst = v_src1 | instr.asI.imm;
}

void FuncInstr::xori() {
    v_dst = v_src1 ^ instr.asI.imm;
}

void FuncInstr::beq() {
    if ( v_src1 == v_src2)
        PC_new += ( instr.asI.imm << 2);
}

void FuncInstr::bne() {
    if ( v_src1 != v_src2)
        PC_new += ( instr.asI.imm << 2);
}

void FuncInstr::j() {
    PC_new = ( PC_old & 0xf0000000) | ( instr.asJ.imm << 2);
}

void FuncInstr::jr() {
    PC_new = v_src1;
}

void FuncInstr::lb() {
    type = BYTE;
    memOp = load;
    mem_addr = v_src2 + instr.asI.imm;
    i_dst = i_src1;
}

void FuncInstr::lh() {
    type = HWORD;
    memOp = load;
    mem_addr = v_src2 + instr.asI.imm;
    i_dst = i_src1;
}

void FuncInstr::lw() {
    type = WORD;
    memOp = load;
    mem_addr = v_src2 + instr.asI.imm;
    i_dst = i_src1;
}

void FuncInstr::lbu() {
    type = UBYTE;
    memOp = load;
    mem_addr = v_src2 + instr.asI.imm;
    i_dst = i_src1;
}

void FuncInstr::lhu() {
    type = UHWORD;
    memOp = load;
    mem_addr = v_src2 + instr.asI.imm;
    i_dst = i_src1;
}

void FuncInstr::sb() {
    type = BYTE;
    memOp = store;
    mem_addr = v_src2 + instr.asI.imm;
}

void FuncInstr::sh() {
    type = HWORD;
    memOp = store;
    mem_addr = v_src2 + instr.asI.imm;
}

void FuncInstr::sw() {
    type = WORD;
    memOp = store;
    mem_addr = v_src2 + instr.asI.imm;
}

void FuncInstr::preDump()
{
    switch ( format)
    {
        case FORMAT_R:
            DumpR();
            break;
        case FORMAT_I:
            DumpI();
            break;
        case FORMAT_J:
            DumpJ();
            break;
    }
}
