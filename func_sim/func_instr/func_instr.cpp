
// Generic C
#include <stdlib.h>

// Generic C++
#include <string>
#include <sstream>
#include <assert.h>

// uArchSim modules
#include <types.h>
#include <func_instr.h>
#include <reg_file.h>
#include <func_memory.h>

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
    { "add  ", 0,  32, FuncInstr::RTYPE, FuncInstr::ADD,   1, 0, 0, 0, 1, 1, 1},
    { "addu ", 0,  33, FuncInstr::RTYPE, FuncInstr::ADDU,  1, 0, 0, 0, 1, 1, 1},
    { "sub  ", 0,  34, FuncInstr::RTYPE, FuncInstr::SUB,   1, 0, 0, 0, 1, 1, 1},
    { "subu ", 0,  35, FuncInstr::RTYPE, FuncInstr::SUBU,  1, 0, 0, 0, 1, 1, 1},
    { "addi ", 8,  0,  FuncInstr::ITYPE, FuncInstr::ADDI,  0, 1, 0, 0, 1, 0, 1},
    { "addiu", 9,  0,  FuncInstr::ITYPE, FuncInstr::ADDIU, 0, 1, 0, 0, 1, 0, 1},
    { "sll  ", 0,  0,  FuncInstr::RTYPE, FuncInstr::SLL,   1, 0, 1, 0, 0, 1, 1},
    { "srl  ", 0,  2,  FuncInstr::RTYPE, FuncInstr::SRL,   1, 0, 1, 0, 0, 1, 1},
    { "beq  ", 4,  0,  FuncInstr::ITYPE, FuncInstr::BEQ,   0, 1, 0, 0, 1, 0, 1},
    { "bne  ", 5,  0,  FuncInstr::ITYPE, FuncInstr::BNE,   0, 1, 0, 0, 1, 0, 1},
    { "j    ", 2,  0,  FuncInstr::JTYPE, FuncInstr::JMP,   0, 0, 0, 1, 0, 0, 0},
    { "jr   ", 0,  8,  FuncInstr::RTYPE, FuncInstr::JR,    1, 0, 0, 0, 1, 0, 0},
    { "lw   ", 35, 0,  FuncInstr::ITYPE, FuncInstr::LW,    0, 1, 0, 0, 1, 0, 1},
    { "sw   ", 43, 0,  FuncInstr::ITYPE, FuncInstr::SW,    0, 1, 0, 0, 1, 0, 1},
    { "li   ", 43, 0,  FuncInstr::ITYPE, FuncInstr::SW,    0, 1, 0, 0, 1, 0, 1},
    { "lui  ", 15, 0,  FuncInstr::ITYPE, FuncInstr::LUI,   0, 1, 0, 0, 0, 0, 1},
};

const Register register_table[] = 
{
    { 0,   "$zero"},
    { 1,   "$at"},
    { 2,   "$v0"},
    { 3,   "$v1"},
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
                this->shimm = shimm;
            if ( instr.uses_addr) 
                this->addr = addr;
            if ( instr.uses_s_register) 
                this->rs = lookup_register( rs_bits);
            if ( instr.uses_d_register) 
                this->rd = lookup_register( rd_bits);
            if ( instr.uses_t_register) 
                this->rt = lookup_register( rt_bits);

            if (  (this->format == ITYPE 
                && instr.uses_t_register && this->rt.num == 2)
                || this->format == RTYPE 
                && instr.uses_d_register && this->rd.num == 2)
                this->is_terminal_instr = true;
            else 
                this->is_terminal_instr = false;

            return;
        }
    }


    cerr << "ERROR: unknown instruction type (opcode: " 
         << (int)opcode << ", funct: " << (int)func << ")" << endl;
    exit( EXIT_FAILURE);
}

FuncInstr::~FuncInstr()
{

}

string FuncInstr::dump( string indent) const
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
                << "0x" << hex << uppercase << imm;
            break;
        case SLL:
        case SRL:
            oss << this->rd.name << ", " << this->rt.name << ", "
                << (int)this->shimm;
            break;
        case BEQ:
        case BNE:
            oss << this->rs.name << ", " << this->rt.name << ", "
                << "0x" << hex << uppercase << this->imm;
            break;
        case JMP:
            oss << "0x" << hex << uppercase << this->addr;
            break;
        case JR:
            oss << this->rs.name;
            break;
        case SW: 
        case LW:
            oss << this->rt.name 
                << ", " << (int)this->imm << "(" << this->rs.name << ")";
            break;
        case LUI:
            oss << this->rt.name 
                << ", 0x" << hex << uppercase << this->imm;
            break;
        default:
            assert( false); 
    }

    return oss.str();
}

string FuncInstr::dumpWithRegisterValues( RegFile &reg_file, string indent) const
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
        oss << "clear " << this->rt.name
            << "(" << reg_file.read( this->rt.num) << ")";
        return oss.str();
    }
    // MOVE
    if ( this->type == ADDIU && !this->imm) 
    {
        oss << "move  " << this->rt.name 
            << "(" << reg_file.read( this->rt.num) << ")"
            << ", " << this->rs.name
            << "(" << reg_file.read( this->rs.num) << ")";
        return oss.str();
    }

    oss << this->name << ' ';
    switch ( this->type) 
    {
        case ADD: 
        case ADDU: 
        case SUB:
        case SUBU:
            oss << this->rd.name 
                << "(" << reg_file.read( this->rd.num) << ")"
                << ", " << this->rs.name 
                << "(" << reg_file.read( this->rs.num) << ")" 
                << ", " << this->rt.name 
                << "(" << reg_file.read( this->rt.num) << ")";
            break;
        case ADDI: 
        case ADDIU:
            oss << this->rt.name 
                << "(" << reg_file.read( this->rt.num) << ")"
                << ", " << this->rs.name 
                << "(" << reg_file.read( this->rs.num) << ")"
                << ", " << "0x" << hex << uppercase << this->imm;
            break;
        case SLL:
        case SRL:
            oss << this->rd.name 
                << "(" << reg_file.read( this->rd.num) << ")"
                << ", " << this->rt.name
                << "(" << reg_file.read( this->rt.num) << ")"
                << ", " << (int)this->shimm;
            break;
        case BEQ:
        case BNE:
            oss << this->rs.name 
                << "(" << reg_file.read( this->rs.num) << ")"
                << ", " << this->rt.name
                << "(" << reg_file.read( this->rt.num) << ")"
                << ", " << "0x" << hex << uppercase << this->imm;
            break;
        case JMP:
            oss << "0x" << hex << uppercase << this->addr;
            break;
        case JR:
            oss << this->rs.name << "(" << reg_file.read( this->rs.num) << ")";
            break;
        case SW: 
        case LW:
            oss << this->rt.name << "(" << reg_file.read( this->rt.num) << ")"
                << ", " << (int)this->imm << "(" << this->rs.name 
                << "(" << reg_file.read( this->rs.num) << "))";
            break;
        case LUI:
            oss << this->rt.name << "(" << reg_file.read( this->rt.num) << ")" 
                << ", 0x" << hex << uppercase << this->imm;
            break;
        default:
            assert( false); 
    }

    return oss.str();
}

uint64 FuncInstr::execute( uint64 pc, FuncMemory &memory, RegFile &regfile) const
{
    switch (this->type)
    {
        case ADD:
        {
            uint64 t = regfile.read( this->rt.num);
            uint64 s = regfile.read( this->rs.num);
            sint64 d = (sint64)t + (sint64)s;
            regfile.write( this->rd.num, d);
            break;
        }
        case ADDU:
        {
            uint64 t = regfile.read( this->rt.num);
            uint64 s = regfile.read( this->rs.num);
            uint64 d = t + s;
            regfile.write( this->rd.num, d);
            break;
        }
        case SUB:
        {
            uint64 t = regfile.read( this->rt.num);
            uint64 s = regfile.read( this->rs.num);
            sint64 d = (sint64)t - (sint64)s;
            regfile.write( this->rd.num, d);
            break;
        }
        case SUBU:
        {
            uint64 t = regfile.read( this->rt.num);
            uint64 s = regfile.read( this->rs.num);
            uint64 d = t - s;
            regfile.write( this->rd.num, d);
            break;
        }
        case ADDI:
        {
            uint64 s = regfile.read( this->rs.num);
            sint64 t = (sint64)s + sint64(this->imm);
            regfile.write( this->rt.num, t);
            break;
        }
        case ADDIU:
        {
            uint64 s = regfile.read( this->rs.num);
            uint64 t = s + this->imm;
            regfile.write( this->rt.num, t);
            break;
        }
        case SLL:
        {
            uint64 t = regfile.read( this->rt.num);
            uint64 d = t << this->shimm;
            regfile.write( this->rd.num, d);
            break;
        }
        case SRL:
        {
            uint64 t = regfile.read( this->rt.num);
            uint64 d = t >> this->shimm;
            regfile.write( this->rd.num, d);
            break;
        }
        case JMP:
            return (pc & 0xf0000000) | (4 * this->imm);
        case JR:
        {
            uint64 s = regfile.read( this->rs.num);
            return s;
        }
        case BEQ:
        {
            uint64 s = regfile.read( this->rs.num);
            uint64 t = regfile.read( this->rt.num);
            if ( t == s)
                return pc + 4 + 4 * (sint16)this->imm;
            break;
        }
        case BNE:
        {
            uint64 s = regfile.read( this->rs.num);
            uint64 t = regfile.read( this->rt.num);
            if ( t != s)
                return pc + 4 + 4 * (sint16)this->imm;
            break;
        }
        case LW:
            regfile.write( this->rt.num, 
                memory.read( regfile.read( this->rs.num) + this->imm, 4));
            break;
        case SW:
            memory.write( regfile.read( this->rt.num), 
                regfile.read( this->rs.num) + this->imm, 4);
            break;
        case LUI:
            regfile.write( this->rt.num, (((uint32)this->imm) << 16));
            break;
    }

    return pc + 4;
}

bool FuncInstr::isTerminalInstruction() const
{
    return this->is_terminal_instr;
}
