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

void FuncInstr::execute(){
    (this->*isaTable[isaNum].func)();
}

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name  opcode  func   format    operation     ldst_size   pointer_to_func
    { "add",    0x0, 0x20,  FORMAT_R, OUT_R_ARITHM, NOT_LDST,   &FuncInstr::add },
    { "addu",   0x0, 0x21,  FORMAT_R, OUT_R_ARITHM, NOT_LDST,   &FuncInstr::add },
    { "sub",    0x0, 0x22,  FORMAT_R, OUT_R_ARITHM, NOT_LDST,   &FuncInstr::sub },
    { "subu",   0x0, 0x23,  FORMAT_R, OUT_R_ARITHM, NOT_LDST,   &FuncInstr::sub },
    { "addi",   0x8, 0x0,   FORMAT_I, OUT_I_ARITHM, NOT_LDST,   &FuncInstr::addi},
    { "addiu",  0x9, 0x0,   FORMAT_I, OUT_I_ARITHM, NOT_LDST,   &FuncInstr::addi},
    { "sll",    0x0, 0x0,   FORMAT_R, OUT_R_SHAMT,  NOT_LDST,   &FuncInstr::sll },
    { "srl",    0x0, 0x2,   FORMAT_R, OUT_R_SHAMT,  NOT_LDST,   &FuncInstr::srl },
    { "slli",   0x0, 0x0,   FORMAT_R, OUT_R_SHAMT,  NOT_LDST,   &FuncInstr::slli},
    { "srli",   0x0, 0x2,   FORMAT_R, OUT_R_SHAMT,  NOT_LDST,   &FuncInstr::srli},
    { "beq",    0x4, 0x0,   FORMAT_I, OUT_I_BRANCH, NOT_LDST,   &FuncInstr::beq },
    { "bne",    0x5, 0x0,   FORMAT_I, OUT_I_BRANCH, NOT_LDST,   &FuncInstr::bne },
    { "j",      0x2, 0x0,   FORMAT_J, OUT_J_JUMP,   NOT_LDST,   &FuncInstr::j   },
    { "jr",     0x0, 0x8,   FORMAT_R, OUT_R_JUMP,   NOT_LDST,   &FuncInstr::jr  },
    { "lui",    0xF, 0x0,   FORMAT_I, OUT_I_SHIFT,  NOT_LDST,   &FuncInstr::lui },
    { "and",    0x0, 0x24,  FORMAT_R, OUT_R_LOG,    NOT_LDST,   &FuncInstr::_and},
    { "or",     0x0, 0x25,  FORMAT_R, OUT_R_LOG,    NOT_LDST,   &FuncInstr::_or },
    { "xor",    0x0, 0x26,  FORMAT_R, OUT_R_LOG,    NOT_LDST,   &FuncInstr::_xor},
    { "andi",   0xC, 0x0,   FORMAT_I, OUT_I_LOG,    NOT_LDST,   &FuncInstr::andi},
    { "ori",    0xD, 0x0,   FORMAT_I, OUT_I_LOG,    NOT_LDST,   &FuncInstr::ori },
    { "xori",   0xE, 0x0,   FORMAT_I, OUT_I_LOG,    NOT_LDST,   &FuncInstr::xori},
    { "lb",     0x20,0x0,   FORMAT_I, OUT_I_LOAD,   BYTE,       &FuncInstr::nop },
    { "lh",     0x21,0x0,   FORMAT_I, OUT_I_LOAD,   HWORD,      &FuncInstr::nop },
    { "lw",     0x23,0x0,   FORMAT_I, OUT_I_LOAD,   WORD,       &FuncInstr::nop },
    { "lbu",    0x24,0x0,   FORMAT_I, OUT_I_LOAD,   UBYTE,      &FuncInstr::nop },
    { "lhu",    0x25,0x0,   FORMAT_I, OUT_I_LOAD,   UHWORD,     &FuncInstr::nop },
    { "sb",     0x28,0x0,   FORMAT_I, OUT_I_STORE,  BYTE,       &FuncInstr::sb  },
    { "sh",     0x29,0x0,   FORMAT_I, OUT_I_STORE,  HWORD,      &FuncInstr::sh  },
    { "sw",     0x2B,0x0,   FORMAT_I, OUT_I_STORE,  WORD,       &FuncInstr::sw  }
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

FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr(bytes)
{
    initFormat();
    new_PC = PC + 4;
    this->PC = PC;
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

int FuncInstr::get_dst_num_index() const{
    switch ( format)
    {
        case FORMAT_R:
            return (int)instr.asR.rd;
            break;

        case FORMAT_I:
            return (int)instr.asI.rt;
            break;

        default:
            break;
    }
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
        case OUT_I_SHIFT:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_LOG:
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_LOAD:
            oss << regTable[instr.asI.rt] << ", "
                << static_cast< signed int>( instr.asI.imm) << "($"
                << regTable[instr.asI.rs] << ")";
            break;
        case OUT_I_STORE:
            oss << regTable[instr.asI.rt] << ", "
                << static_cast< signed int>( instr.asI.imm) << "($"
                << regTable[instr.asI.rs] << ")";
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
