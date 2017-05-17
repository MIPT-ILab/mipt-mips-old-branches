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
    // name  opcode   func   format    operation     method
    { "add",    0x0,  0x20,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::add           },
    { "addu",   0x0,  0x21,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::add           },
    { "sub",    0x0,  0x22,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::sub           },
    { "subu",   0x0,  0x23,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::sub           },
    { "addi",   0x8,  0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addi          },
    { "addiu",  0x9,  0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addi          },
    { "sll",    0x0,  0x0,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::left_shift    },
    { "srl",    0x0,  0x2,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::right_shift   },
    { "beq",    0x4,  0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::beq           }, 
    { "bne",    0x5,  0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::bne           },
    { "j",      0x2,  0x0,   FORMAT_J, OUT_J_JUMP,   &FuncInstr::jump          },
    { "jr",     0x0,  0x8,   FORMAT_R, OUT_R_JUMP,   &FuncInstr::jumpr         },
    { "lui",    0xF,  0x0,   FORMAT_I, OUT_I_SHAMT,  &FuncInstr::left_shift_16 },
    { "and",    0x0,  0x24,  FORMAT_R, LOGICAL_R,    &FuncInstr::bit_and       },
    { "or",     0x0,  0x25,  FORMAT_R, LOGICAL_R,    &FuncInstr::bit_or        },
    { "xor",    0x0,  0x26,  FORMAT_R, LOGICAL_R,    &FuncInstr::bit_xor       },
    { "andi",   0xC,  0x0,   FORMAT_I, LOGICAL_I,    &FuncInstr::bit_andi      },
    { "ori",    0xD,  0x0,   FORMAT_I, LOGICAL_I,    &FuncInstr::bit_ori       },
    { "xori",   0xE,  0x0,   FORMAT_I, LOGICAL_I,    &FuncInstr::bit_xori      },
    { "lb",     0x20, 0x0,   FORMAT_I, LOAD,         &FuncInstr::load8         },
    { "lh",     0x21, 0x0,   FORMAT_I, LOAD,         &FuncInstr::load16        },
    { "lw",     0x23, 0x0,   FORMAT_I, LOAD,         &FuncInstr::load32        },
    { "lbu",    0x24, 0x0,   FORMAT_I, LOAD,         &FuncInstr::load8         },
    { "lhu",    0x25, 0x0,   FORMAT_I, LOAD,         &FuncInstr::load16        },
    { "sb",     0x28, 0x0,   FORMAT_I, STORE,        &FuncInstr::store8        },
    { "sh",     0x29, 0x0,   FORMAT_I, STORE,        &FuncInstr::store16       },
    { "sw",     0x2b, 0x0,   FORMAT_I, STORE,        &FuncInstr::store32       }
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

FuncInstr::FuncInstr(uint32 bytes, uint32 _PC): 
instr(bytes), 
PC(_PC), 
v_src1(0),
v_src2(0), 
v_dst(0), 
mem_addr(0),
mem_num_bytes(0),
new_PC(_PC),
is_load(true)
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
        case LOGICAL_R:
            oss << regTable[instr.asR.rd] << ", $" \
                << regTable[instr.asR.rt] << ", $" \
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
        case LOGICAL_I:
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case LOAD:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec
                << "(" << regTable[instr.asI.rs] << ")";
            break;
        case STORE:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec
                << "(" << regTable[instr.asI.rs] << ")";
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

void FuncInstr::load8()  
{ 
    mem_addr = v_src1 + instr.asI.imm; 
    mem_num_bytes = 1;
    is_load = true; 
}

void FuncInstr::load16() 
{ 
    mem_addr = v_src1 + instr.asI.imm; 
    mem_num_bytes = 2;
    is_load = true; 
}

void FuncInstr::load32() 
{ 
    mem_addr = v_src1 + instr.asI.imm; 
    mem_num_bytes = 4;
    is_load = true; 
}

void FuncInstr::store8()
{   
    v_dst = v_src2 & 0xff;
    mem_addr = v_src1 + instr.asI.imm;
    is_load = false;
    mem_num_bytes = 1;
}

void FuncInstr::store16()
{   
    v_dst = v_src2 & 0xffff;
    mem_addr = v_src1 + instr.asI.imm;
    is_load = false;
    mem_num_bytes = 2;
}

void FuncInstr::store32()
{   
    v_dst = v_src2;
    mem_addr = v_src1 + instr.asI.imm;
    is_load = false;
    mem_num_bytes = 4;
}

void FuncInstr::execute()
{ 
    (isaTable[isaNum].*method)();
    if (operation != OUT_R_JUMP && operation != OUT_J_JUMP)
        new_PC += 4;
}

RegNum FuncInstr::get_src1_num_index() const
{
    switch (operation)
    {
        case OUT_R_ARITHM: 
        case OUT_I_ARITHM:
        case LOGICAL_R:
        case LOGICAL_I:
        case OUT_I_BRANCH:
        case OUT_R_JUMP:
        case LOAD:
        case STORE:
            return (RegNum) instr.asR.rs;
        case OUT_R_SHAMT:
            return (RegNum) instr.asR.rt;
        default:
            return (RegNum) MAX_REG;
    }    
}

RegNum FuncInstr::get_src2_num_index() const
{
    switch (operation)
    {
        case OUT_R_ARITHM:
        case LOGICAL_R:
        case OUT_I_BRANCH:
        case STORE:
            return (RegNum) instr.asR.rt;
        default:
            return (RegNum) MAX_REG;
    }
}

RegNum FuncInstr::get_dst_num_index() const
{
    switch (operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_SHAMT:
        case LOGICAL_R:
            return (RegNum) instr.asR.rd;
        case OUT_I_ARITHM:
        case OUT_I_SHAMT:
        case LOGICAL_I:
        case LOAD:
            return (RegNum) instr.asR.rt;
        default:
            return (RegNum) MAX_REG;
    }
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

