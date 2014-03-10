/*
 * func_instr.cpp - instruction parser for mips
 * @author Aleksandr Kayda aleksandr.kajda@frtk.ru
 * Copyright 2014 uArchSim iLab 
 */


#include <func_instr.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>


const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name  opcode  func   format    outputType
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
    { "j",      0x0, 0x0,   FORMAT_J, OUT_J_JUMP},
    { "jr",     0x0, 0x8,   FORMAT_R, OUT_R_JUMP}
};
const uint32 FuncInstr::isaTableSize = 12;

const char *FuncInstr::regTable[32] = 
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



FuncInstr::FuncInstr( uint32 bytes)
{
    instr.raw = bytes;
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
    return disasmStr;
}


void FuncInstr::initFormat()
{
    int i;
    for ( i = 0; i < isaTableSize; i++)
        if ( instr.asR.opcode == isaTable[i].opcode)
        {
            format = isaTable[i].format;
            break;
        }
    if ( i == isaTableSize)
        format = FORMAT_UNKNOWN;   // unknown format
}


void FuncInstr::initR()
{
    int i;
    for ( i = 0; i < isaTableSize; i++)
        if (( instr.asR.opcode == isaTable[i].opcode) &&
            ( instr.asR.funct == isaTable[i].funct))
        {
            outputType = isaTable[i].outputType;
            isaNum = i;
            break;
        }
    if ( i == isaTableSize)
    {
        initUnknown();
        return;
    }
    if ( checkPseudo())
        return;
    ostringstream oss;
    oss << hex << setfill('0');
    oss << "0x" << setw(8) << instr.raw << '\t' << isaTable[isaNum].name << " $";
    switch (outputType)
    {
        case OUT_R_ARITHM:
            oss << regTable[instr.asR.rd] << ", $" \
                << regTable[instr.asR.rs] << ", $" \
                << regTable[instr.asR.rt];
            break;
        case OUT_R_SHAMT:
            oss << regTable[instr.asR.rd] << ", $" \
                << regTable[instr.asR.rt] << ", " \
                << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << regTable[instr.asR.rs];
            break;
    }
    oss << endl;
    disasmStr = oss.str();
}


void FuncInstr::initI()
{
    for ( int i = 0; i < isaTableSize; i++)
        if ( instr.asI.opcode == isaTable[i].opcode) 
        {
            outputType = isaTable[i].outputType;
            isaNum = i;
            break;
        }
    if ( checkPseudo())
        return;
    ostringstream oss;
    oss << hex << setfill('0');
    oss << "0x" << setw(8) << instr.raw << '\t' << isaTable[isaNum].name << " $";
    switch (outputType)
    {
        case OUT_I_ARITHM:
            oss << regTable[instr.asI.rt] << ", $" \
                << regTable[instr.asI.rs] << ", " \
                << dec << (signed short) instr.asI.imm;
            break;
        case OUT_I_BRANCH:
            oss << regTable[instr.asI.rs] << ", $" \
                << regTable[instr.asI.rt] << ", " \
                << dec << (signed short) instr.asI.imm;
            break;
    }
    oss << endl;
    disasmStr = oss.str();
}


void FuncInstr::initJ()
{
    for ( int i = 0; i < isaTableSize; i++)
        if ( instr.asJ.opcode == isaTable[i].opcode) 
        {
            outputType = isaTable[i].outputType;
            isaNum = i;
            break;
        }
    if ( checkPseudo())
        return;
    ostringstream oss;
    oss << hex << setfill('0');
    oss << "0x" << setw(8) << instr.raw << '\t' << isaTable[isaNum].name << " ";
    oss << instr.asJ.imm;
    oss << endl;
    disasmStr = oss.str();
}


void FuncInstr::initUnknown()
{
    ostringstream oss;
    oss << hex << setfill('0');
    oss << "0x" << setw(8) << instr.raw << '\t' << "Unknown" << endl;
    disasmStr = oss.str();
}


int FuncInstr::checkPseudo()
{
    ostringstream oss;
    oss << hex << setfill('0');
    oss << "0x" << setw(8) << instr.raw << '\t';
    // nop
    if ( instr.raw == 0)
    {
        oss << "nop" << endl;
        disasmStr = oss.str();
        return 1;
    }
    // move
    if (( instr.asI.opcode == 0x9) && ( instr.asI.imm == 0))
    {
        oss << "move" << " $" << regTable[instr.asI.rt] \
            << ", $" << regTable[instr.asI.rs] << endl;
        disasmStr = oss.str();
        return 1;
    }
    // move
    if (( instr.asR.opcode == 0x0) && ( instr.asR.funct == 0x21) 
            && (instr.asR.rt == 0))
    {
        oss << "move" << " $" << regTable[instr.asR.rd] \
            << ", $" << regTable[instr.asI.rs] << endl;
        disasmStr = oss.str();
        return 1;
    }
    // clear
    if (( instr.asR.opcode == 0x0) && ( instr.asR.funct == 0x21) 
            && (instr.asR.rs == 0) && ( instr.asR.rt == 0))
    {
        oss << "clear" << " $" << regTable[instr.asR.rd] << endl;
        disasmStr = oss.str();
        return 1;
    }
    return 0;
}


std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
         return out << instr.Dump("");
}



