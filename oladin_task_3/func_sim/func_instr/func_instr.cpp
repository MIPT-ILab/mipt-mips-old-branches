/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

/**
 * Edited by Ladin Oleg.
 *
 * Note: "init" methods was changed. Now "disasm" string carries output in
 * this form: "add $t1 [08], $t2 [09s], $t3 [10s]". In square brackets there
 * is number of register, "s" means that its value stored in "v_src1", "t" -
 * in "v_src2".
 */

#include <func_instr.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <cstring> // for strcmp()

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name   opcode func  format    operation     function        properties
    { "add",   0x0,  0x20, FORMAT_R, OUT_R_ARITHM, &FuncInstr::add,   0x0  },
    { "addu",  0x0,  0x21, FORMAT_R, OUT_R_ARITHM, &FuncInstr::addu,  0x0  },
    { "sub",   0x0,  0x22, FORMAT_R, OUT_R_ARITHM, &FuncInstr::sub,   0x0  },
    { "subu",  0x0,  0x23, FORMAT_R, OUT_R_ARITHM, &FuncInstr::subu,  0x0  },
    { "addi",  0x8,  0x0,  FORMAT_I, OUT_I_ARITHM, &FuncInstr::addi,  0x0  },
    { "addiu", 0x9,  0x0,  FORMAT_I, OUT_I_ARITHM, &FuncInstr::addiu, 0x0  },
    { "sll",   0x0,  0x0,  FORMAT_R, OUT_R_SHAMT,  &FuncInstr::sll,   0x0  },
    { "srl",   0x0,  0x2,  FORMAT_R, OUT_R_SHAMT,  &FuncInstr::srl,   0x0  },
    { "beq",   0x4,  0x0,  FORMAT_I, OUT_I_BRANCH, &FuncInstr::beq,   0x1  },
    { "bne",   0x5,  0x0,  FORMAT_I, OUT_I_BRANCH, &FuncInstr::bne,   0x1  },
    { "j",     0x2,  0x0,  FORMAT_J, OUT_J_JUMP,   &FuncInstr::j,     0x1  },
    { "jr",    0x0,  0x8,  FORMAT_R, OUT_R_JUMP,   &FuncInstr::jr,    0x1  },
    { "lui",   0xf,  0x0,  FORMAT_I, OUT_I_LOAD,   &FuncInstr::lui,   0x0  },
    { "and",   0x0,  0x24, FORMAT_R, OUT_R_LOG,    &FuncInstr::and_,  0x0  },
    { "or",    0x0,  0x25, FORMAT_R, OUT_R_LOG,    &FuncInstr::or_,   0x0  },
    { "xor",   0x0,  0x26, FORMAT_R, OUT_R_LOG,    &FuncInstr::xor_,  0x0  },
    { "andi",  0xc,  0x0,  FORMAT_I, OUT_I_LOG,    &FuncInstr::andi,  0x0  },
    { "ori",   0xd,  0x0,  FORMAT_I, OUT_I_LOG,    &FuncInstr::ori,   0x0  },
    { "xori",  0xe,  0x0,  FORMAT_I, OUT_I_LOG,    &FuncInstr::xori,  0x0  },
    { "lb",    0x20, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::lb,    0x12 },
    { "lh",    0x21, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::lh,    0x16 },
    { "lw",    0x23, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::lw,    0x1a },
    { "lbu",   0x24, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::lbu,   0x10 },
    { "lhu",   0x25, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::lhu,   0x14 },
    { "sb",    0x28, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::sb,    0x81 },
    { "sh",    0x29, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::sh,    0xa1 },
    { "sw",    0x2b, 0x0,  FORMAT_I, OUT_I_MEM,    &FuncInstr::sw,    0xc1 }
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

FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr( bytes), PC( PC)
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
            new_PC = PC + 4; // next instruction by default
            func = isaTable[ i].func; // saves method to execute
            prop.num = isaTable[ i].properties; // saves access properties
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
            oss << regTable[ instr.asR.rd] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rd << "]" << ", $"
                << regTable[ instr.asR.rs] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rs << "s]" << ", $"
                << regTable[ instr.asR.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rt << "t]";
            break;
        case OUT_R_SHAMT:
            v_src1 = instr.asR.shamt;
            oss << regTable[ instr.asR.rd] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rd << "]" << ", $"
                << regTable[ instr.asR.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rt << "t]" << ", "
                << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << regTable[ instr.asR.rs] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rs << "]";
            break;
        case OUT_R_LOG:
            oss << regTable[ instr.asR.rd] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rd << "]" << ", $"
                << regTable[ instr.asR.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rt << "t]" << ", $"
                << regTable[ instr.asR.rs] << " [" << setfill( '0') << setw( 2)
                << instr.asR.rs << "s]";
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
            if ( !strcmp( isaTable[ isaNum].name.data(), "addi")) // addi
            {
                v_src2 = static_cast< int16>( instr.asI.imm);
            } else // addiu
            {
                v_src2 = static_cast< uint16>( instr.asI.imm);
            }
            oss << regTable[ instr.asI.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rt << "]" << ", $"
                << regTable[ instr.asI.rs] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rs << "s]" << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << dec;
            break;
        case OUT_I_BRANCH:
            v_dst = instr.asI.imm;
            oss << regTable[ instr.asI.rs] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rs << "]" << ", $"
                << regTable[ instr.asI.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rt << "]" << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << dec;
            break;
        case OUT_I_LOAD:
            v_src1 = instr.asI.imm;
            v_src2 = 16;
            oss << regTable[ instr.asI.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rt << "]" << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << dec;
            break;
        case OUT_I_LOG:
            v_src2 = instr.asI.imm;
            oss << regTable[ instr.asI.rs] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rs << "s]" << ", $"
                << regTable[ instr.asI.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rt << "]" << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << dec;
            break;
        case OUT_I_MEM:
            v_src2 = instr.asI.imm;
            oss << regTable[ instr.asI.rt] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rt << "]" << ", "
                << hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << dec << "($"
                << regTable[ instr.asI.rs] << " [" << setfill( '0') << setw( 2)
                << instr.asI.rs << "s])";
            break;
    }
    disasm = oss.str();
}

void FuncInstr::initJ()
{
    v_src1 = instr.asJ.imm;
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

