
#include <string>
#include <sstream>
#include <assert.h>

#include "func_instr.h"

using namespace std;

struct InstrTableEntry
{
    const char* name;

    uint8 opcode;
    uint8 func;

    FuncInstr::FormatType format;
    FuncInstr::Type type;

    bool uses_func;
    bool uses_imm;
    bool uses_shimm;
    bool uses_addr;
    bool uses_s_register;
    bool uses_d_register;
    bool uses_t_register;
};

const InstrTableEntry instr_table[] = 
{
    { "add  ", 0, 32, FuncInstr::RTYPE, FuncInstr::ADD,   1, 0, 0, 0, 1, 1, 1},
    { "addu ", 0, 33, FuncInstr::RTYPE, FuncInstr::ADDU,  1, 0, 0, 0, 1, 1, 1},
    { "sub  ", 0, 34, FuncInstr::RTYPE, FuncInstr::SUB,   1, 0, 0, 0, 1, 1, 1},
    { "subu ", 0, 35, FuncInstr::RTYPE, FuncInstr::SUBU,  1, 0, 0, 0, 1, 1, 1},
    { "addi ", 8, 0,  FuncInstr::ITYPE, FuncInstr::ADDI,  0, 1, 0, 0, 1, 0, 1},
    { "addiu", 9, 0,  FuncInstr::ITYPE, FuncInstr::ADDIU, 0, 1, 0, 0, 1, 0, 1},
    { "sll  ", 0, 0,  FuncInstr::RTYPE, FuncInstr::SLL,   1, 0, 1, 0, 0, 1, 1},
    { "srl  ", 0, 2,  FuncInstr::RTYPE, FuncInstr::SRL,   1, 0, 1, 0, 0, 1, 1},
    { "beq  ", 4, 0,  FuncInstr::ITYPE, FuncInstr::BEQ,   0, 1, 0, 0, 1, 0, 1},
    { "bne  ", 5, 0,  FuncInstr::ITYPE, FuncInstr::BNE,   0, 1, 0, 0, 1, 0, 1},
    { "j    ", 2, 0,  FuncInstr::JTYPE, FuncInstr::JMP,   0, 0, 0, 1, 0, 0, 0},
    { "jr   ", 0, 8,  FuncInstr::RTYPE, FuncInstr::JRE,   1, 0, 0, 0, 1, 0, 0},
};

const Register register_table[] = 
{
    { 0,   "$zero"},
    { 1,   "$at"},
    { 8,   "$t0"},
    { 9,   "$t1"},
    { 10,  "$t2"},
    { 11,  "$t3"},
    { 12,  "$t4"},
    { 13,  "$t5"},
    { 14,  "$t6"},
    { 15,  "$t7"},
    { 24,  "$t8"},
    { 25,  "$t9"},
};

const size_t register_table_size = 
    sizeof(register_table) / sizeof(register_table[0]);
const size_t instr_table_size = sizeof(instr_table) / sizeof(instr_table[0]);

Register lookup_register( uint8 num)
{
    for ( size_t i = 0; i < register_table_size; ++i) 
        if ( register_table[i].num == num) 
            return register_table[i];
    assert( false);
}

FuncInstr::FuncInstr( uint32 bits)
{
    uint8  opcode = ( bits & 0xfc000000) >> 26;
    uint8    func = ( bits & 0x3f);
    uint32   addr = ( bits & 0x3fffff) << 2;
    uint16    imm = ( bits & 0xffff);
    uint8   shimm = ( bits & 0x7c0) >> 6;
    uint8 rs_bits = ( bits & 0x3e00000) >> 21;
    uint8 rt_bits = ( bits & 0x1f0000) >> 16;
    uint8 rd_bits = ( bits & 0xf800) >> 11;

    for ( size_t i = 0; i < instr_table_size; ++i) 
    {
        InstrTableEntry instr = instr_table[i];

        if (instr.opcode == opcode 
             && (!instr.uses_func || instr.uses_func && instr.func == func))
        {
            this->name   = instr.name;
            this->type   = instr.type;
            this->format = instr.format;

            if ( instr.uses_imm) 
                this->imm = imm;
            if ( instr.uses_shimm) 
                this->imm = shimm;
            if ( instr.uses_addr) 
                this->addr = addr;
            if ( instr.uses_s_register) 
                this->rs = lookup_register( rs_bits);
            if ( instr.uses_d_register) 
                this->rd = lookup_register( rd_bits);
            if ( instr.uses_t_register) 
                this->rt = lookup_register( rt_bits);

            return;
        }
    }

    assert( false);
}

FuncInstr::~FuncInstr()
{

}

string FuncInstr::Dump( string indent) const
{
    ostringstream oss;
    oss << indent;

    // Process pseudo-instructions first

    // NOP
    if ( this->type == SLL && !this->imm && !this->rd.num && !this->rt.num) 
    {
        oss << "nop";
        return oss.str();
    }
    // CLEAR
    if ( this->type == ADDU && !this->rs.num && !this->rt.num)
    {
        oss << "clear " << this->rt.name;
        return oss.str();
    }
    // MOVE
    if ( this->type == ADDIU && !this->imm) 
    {
        oss << "move  " << this->rt.name << ", " << this->rs.name;
        return oss.str();
    }

    oss << this->name << ' ';
    switch ( this->type) 
    {
        case ADD: 
        case ADDU: 
        case SUB:
        case SUBU:
            oss << this->rd.name<< ", " << this->rs.name << ", " 
                << this->rt.name;
            break;
        case ADDI: 
        case ADDIU:
            oss << this->rt.name << ", " << this->rs.name << ", "
                << "0x" << hex << imm;
            break;
        case SLL:
        case SRL:
            oss << this->rd.name << ", " << this->rt.name << ", "
                << "0x" << hex << this->imm;
            break;
        case BEQ:
        case BNE:
            oss << this->rs.name << ", " << this->rt.name << ", "
                << "0x" << hex << this->imm;
            break;
        case JMP:
            oss << "0x" << hex << this->addr;
            break;
        case JRE:
            oss << this->rs.name;
            break;
        default:
            assert( false); 
    }

    return oss.str();
}