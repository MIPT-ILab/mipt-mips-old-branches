/**
 * func_instr.cpp - module implementing parsing
 * of 32-bit words-instructions for further use
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Genereic C
#include <stdlib.h>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <types.h>
#include <func_instr.h>

const FuncInstr::InstrInfo_t FuncInstr::InstrInfos[] = 
    {
        {"add",   TYPE_R, INSTR_ADD,   PT_DST_S,  0x00, 0x20},
        {"addu",  TYPE_R, INSTR_ADDU,  PT_DST_U,  0x00, 0x21},
        {"sub",   TYPE_R, INSTR_SUB,   PT_DST_S,  0x00, 0x22},
        {"subu",  TYPE_R, INSTR_SUBU,  PT_DST_U,  0x00, 0x23},
        {"addi",  TYPE_I, INSTR_ADDI,  PT_TSI_S,  0x08, 0x00},
        {"addiu", TYPE_I, INSTR_ADDIU, PT_TSI_U,  0x09, 0x00},
        {"sll",   TYPE_R, INSTR_SLL,   PT_DTSH_U, 0x00, 0x00},
        {"srl",   TYPE_R, INSTR_SRL,   PT_DTSH_U, 0x00, 0x02},
        {"beq",   TYPE_I, INSTR_BEQ,   PT_STI_S,  0x04, 0x00},
        {"bne",   TYPE_I, INSTR_BNE,   PT_STI_S,  0x05, 0x00},
        {"j",     TYPE_J, INSTR_J,     PT_ADDR_U, 0x02, 0x00},
        {"jr",    TYPE_R, INSTR_JR,    PT_S_U,    0x00, 0x08},
        {"lw",    TYPE_I, INSTR_LW,    PT_TIS_S,  0x23, 0x00},
        {"sw",    TYPE_I, INSTR_SW,    PT_TIS_S,  0x2B, 0x00},
        {"lui",   TYPE_I, INSTR_LUI,   PT_TI_U,   0x0F, 0x00},
    };
const int FuncInstr::InstrInfosSize = sizeof( FuncInstr::InstrInfos) / sizeof( FuncInstr::InstrInfos[0]);

const std::string FuncInstr::RegNames[] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
                                           "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
                                           "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                                           "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
const int FuncInstr::RegNamesSize = sizeof( FuncInstr::RegNames) / sizeof( FuncInstr::RegNames[0]);


void FuncInstr::DefineType()
{
    // Defines type of instruction ( R, I or J)
    // and opcode on the side
    opcode = instrBytes >> 26;
    for( int i = 0; i < InstrInfosSize; i++)
    {
        if( InstrInfos[ i].opcode == opcode)
        {
            type = InstrInfos[ i].type;
            return;
        }
    }

    std::cerr << "ERROR: unsupported instruction." << std::endl;
    exit( EXIT_FAILURE);
}
    
void FuncInstr::DefineInnnerFields()
{
    // Defines funct, shamt, immediate, address and 
    // registers depending on type of instruction
    InstrParser_t instrParser;
    instrParser.bytes = instrBytes;
    switch( type)
    {
        case TYPE_R:
            funct = instrParser.asR.funct;
            shamt = instrParser.asR.shamt;
            d_reg = instrParser.asR.d_reg;
            t_reg = instrParser.asR.t_reg;
            s_reg = instrParser.asR.s_reg;
            break;

        case TYPE_I:
            immed = instrParser.asI.immed;
            t_reg = instrParser.asI.t_reg;
            s_reg = instrParser.asI.s_reg;
            funct = 0x00;
            break;

        case TYPE_J:
            addr = instrParser.asJ.addr;
            funct = 0x00;
            break;
    }
}

void FuncInstr::DefineInstr()
{
    // Defines instruction: its name and the way it prints
    for( int i = 0; i < InstrInfosSize; i++)
    {
        if( InstrInfos[ i].opcode == opcode && InstrInfos[ i].funct == funct)
        {
            name      = InstrInfos[ i].name;
            instr     = InstrInfos[ i].instr;
            printType = InstrInfos[ i].printType;
            return;
        }
    }

    std::cerr << "ERROR: unsupported instruction." << std::endl;
    exit( EXIT_FAILURE);
}

void FuncInstr::DefinePseudoInstr()
{
    // Defines if the instruction is pseudo
    if( instrBytes == 0)
    {
        pseudoInstr = PINSTR_NOP;
        printType   = PT_NAME;
        name        = "nop";
        return;
    } else if( instrBytes & 0xffe0f83f == 0x00000021)
    {
        pseudoInstr = PINSTR_CLEAR;
        printType   = PT_T_U;
        name        = "clear";
        return;
    } else if( instrBytes & 0xfc00ffff == 0x24000000)
    {
        pseudoInstr = PINSTR_MOVE;
        printType   = PT_TS_U;
        name        = "move";
        return;
    }

    pseudoInstr = PINSTR_NON_PSEUDO;
}

FuncInstr::FuncInstr( uint32 bytes)
{
    instrBytes = bytes;
    DefineType();
    DefineInnnerFields();
    DefineInstr();
    DefinePseudoInstr();
}

std::string FuncInstr::Dump( std::string indent) const
{
    // Returns assembler representation of instruction
    std::ostringstream oss;
    oss << indent << name << " ";
    switch( printType)
    {
        case PT_DST_S:
        case PT_DST_U:
            oss << RegNames[d_reg] << ", " << RegNames[s_reg] << ", " << RegNames[t_reg];
            break;

        case PT_TSI_S:
        case PT_TSI_U:
            oss << RegNames[t_reg] << ", " << RegNames[s_reg] << ", 0x" << std::hex << immed;
            break;

        case PT_DTSH_U:
            oss << RegNames[d_reg] << ", " << RegNames[t_reg] << ", 0x" << std::hex << shamt;
            break;

        case PT_STI_S:
            oss << RegNames[s_reg] << ", " << RegNames[t_reg] << ", 0x" << std::hex << immed;
            break;

        case PT_ADDR_U:
            oss << "0x" << std::hex << addr;
            break;

        case PT_S_U:
            oss << RegNames[s_reg];
            break;

        case PT_TS_U:
            oss << RegNames[t_reg] << ", " << RegNames[s_reg];
            break;

        case PT_T_U:
            oss << RegNames[t_reg];
            break;

        case PT_NAME:
            break;

        case PT_TIS_S:
            oss << RegNames[t_reg] << ", 0x" << immed << "(" << RegNames[s_reg] << ")";
            break;

        case PT_TI_U:
            oss << RegNames[t_reg] << ", 0x" << immed;
            break;
        default:
            std::cerr << "ERROR! Unsupporeted print type occured!" << std::endl;
            exit( EXIT_FAILURE);
    }
    return oss.str();
}

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump("");
}

std::string FuncInstr::getName()
{
    return name;
}

std::string FuncInstr::getSRegName()
{
    return RegNames[s_reg];
}

std::string FuncInstr::getTRegName()
{
    return RegNames[t_reg];
}

std::string FuncInstr::getDRegName()
{
    return RegNames[d_reg];
}

FuncInstr::Type FuncInstr::getType()
{
    return type;
}

FuncInstr::PrintType FuncInstr::getPrintType()
{
    return printType;
}

FuncInstr::Instr FuncInstr::getInstr()
{
    return instr;
}

uint8 FuncInstr::getSReg()
{
    return s_reg;
}

uint8 FuncInstr::getTReg()
{
    return t_reg;
}

uint8 FuncInstr::getDReg()
{
    return d_reg;
}

uint16 FuncInstr::getImmed()
{
    return immed;
}

uint8 FuncInstr::getShamt()
{
    return shamt;
}

uint32 FuncInstr::getAddr()
{
    return addr;
}
