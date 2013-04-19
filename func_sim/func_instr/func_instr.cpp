/**
 * func_instr.cpp - Implementation of the instruction decoder 
 * author Alexander Kravtcov
 */

// Generic C
#include <cstdlib>

// Generic C++
#include <string>
#include <iostream>
#include <sstream>

// uArchSim modules
#include <func_instr.h>

const FuncInstr::InstrInfo FuncInstr::ISA[] =
{
    /*instr    name  opcode  funct    type  s  t  d im order*/
    {   ADD, "  add",  0x0,   0x20, R_TYPE, 1, 1, 1, 0, DST},
    {  ADDU, " addu",  0x0,   0x21, R_TYPE, 1, 1, 1, 0, DST},
    {   SUB, "  sub",  0x0,   0x22, R_TYPE, 1, 1, 1, 0, DST},
    {  SUBU, " subu",  0x0,   0x23, R_TYPE, 1, 1, 1, 0, DST},
    {  ADDI, " addi",  0x8, UNUSED, I_TYPE, 1, 1, 0, 1, TSI},
    { ADDIU, "addiu",  0x9, UNUSED, I_TYPE, 1, 1, 0, 1, TSI},
    {   SLL, "  sll",  0x0,    0x0, R_TYPE, 0, 1, 1, 1, DTI},
    {   SRL, "  srl",  0x0,    0x2, R_TYPE, 0, 1, 1, 1, DTI},
    {   BEQ, "  beq",  0x4, UNUSED, I_TYPE, 1, 1, 0, 1, STI},
    {   BNE, "  bne",  0x5, UNUSED, I_TYPE, 1, 1, 0, 1, STI},
    {     J, "    j",  0x2, UNUSED, J_TYPE, 0, 0, 0, 1,   I},
    {    JR, "   jr",  0x0,    0x8, R_TYPE, 1, 0, 0, 0,   S},
    {    LW, "   lw", 0x23, UNUSED, I_TYPE, 1, 1, 0, 1, TIS},
    {    SW, "   sw", 0x2B, UNUSED, I_TYPE, 1, 1, 0, 1, TIS},
    {   LUI, "  lui",  0xF, UNUSED, I_TYPE, 0, 1, 0, 1,  TI},
    {   NOP, "  nop",  0x0,    0x0, R_TYPE, 0, 0, 0, 0, NOT}
};

const FuncInstr::RegName FuncInstr::RegNames[] =
{ 
    { RegFile::ZERO, "$zero"}, { RegFile::AT, "$at"},
    {   RegFile::V0,   "$v0"}, { RegFile::V1, "$v1"},
    {   RegFile::A0,   "$a0"}, { RegFile::A1, "$a1"},
    {   RegFile::A2,   "$a2"}, { RegFile::A3, "$a3"},
    {   RegFile::T0,   "$t0"}, { RegFile::T1, "$t1"},
    {   RegFile::T2,   "$t2"}, { RegFile::T3, "$t3"}, 
    {   RegFile::T4,   "$t4"}, { RegFile::T5, "$t5"},
    {   RegFile::T6,   "$t6"}, { RegFile::T7, "$t7"},
    {   RegFile::S0,   "$s0"}, { RegFile::S1, "$s1"},
    {   RegFile::S2,   "$s2"}, { RegFile::S3, "$s3"}, 
    {   RegFile::S4,   "$s4"}, { RegFile::S5, "$s5"},
    {   RegFile::S6,   "$s6"}, { RegFile::S7, "$s7"}, 
    {   RegFile::T8,   "$t8"}, { RegFile::T9, "$t9"},
    {   RegFile::K0,   "$k0"}, { RegFile::K1, "$k1"}, 
    {   RegFile::GP,   "$gp"}, { RegFile::SP, "$sp"},
    {   RegFile::FP,   "$fp"}, { RegFile::RA, "$ra"}
};


FuncInstr::FuncInstr( uint32 bytes)
{
    this->convertible.bytes = bytes;

    this->instr_info = selection();

    parseInstr();

    this->num_of_arguments = this->instr_info.is_reg_s 
                             + this->instr_info.is_reg_t 
                             + this->instr_info.is_reg_d 
                             + this->instr_info.is_imm;
}

FuncInstr::InstrInfo FuncInstr::selection() const
{

#if DEBUG        
    std::cout << "instr: " << std::hex << this->convertible.bytes << std::dec
              << ", opcode: " << std::hex << this->convertible.asR.opcode 
              << std::dec << ", funct: " << std::hex
              << this->convertible.asR.funct << std::dec << std::endl;
#endif

    for ( int i = 0; i < SIZE_OF_INSTR; i++)
    {
        if ( ( this->convertible.asR.opcode == ISA[ i].opcode) &&
             ( ( ISA[ i].funct == UNUSED) ||
               ( this->convertible.asR.funct == ISA[ i].funct)))
        {
            return ISA[ i];
        }
    }
    
    std::cerr << "ERROR. Instuction 0x" << std::hex << this->convertible.bytes
              << std::dec << " is not a MIPS instruction!" << std::endl;

    exit( EXIT_FAILURE);
}

void FuncInstr::parseInstr()
{
    // for all instructions position of rigesters like R_TYPE
    this->reg_s = RegNames[ this->convertible.asR.s];
    this->reg_t = RegNames[ this->convertible.asR.t];
    this->reg_d = RegNames[ this->convertible.asR.d];

    switch ( this->instr_info.type)
    {
        case R_TYPE:
            this->imm = this->convertible.asR.shamt;
            break;

        case J_TYPE:
            this->imm = this->convertible.asJ.addr;
            break;

        case I_TYPE:
            switch ( this->instr_info.instr)
            {
                case ADDIU:
                case LUI:
                    this->imm = ( uint64)this->convertible.asI.imm;
                    break;
                default:
                    this->imm = ( uint64)( ( sint64)
                                           ( ( short)this->convertible.asI.imm));
                    break;
            }
            break;
    }

    // pseudo-instructions
    if ( ( this->instr_info.instr == ADDIU) && ( this->imm == 0))
    {
        this->instr_info.name_str = " move";
        this->instr_info.is_imm = 0;
        this->instr_info.arg_order = TS;
    }
    if ( ( this->instr_info.instr == ADDU)
         && ( this->reg_t.reg == RegFile::ZERO))
    {
        this->instr_info.name_str = " move";
        this->instr_info.is_reg_t = 0;
        this->instr_info.arg_order = DS;
    }
    if ( ( this->instr_info.instr == ADDU)
         && ( this->reg_s.reg == RegFile::ZERO)
         && ( this->reg_d.reg == RegFile::ZERO))
    {
        this->instr_info.name_str = "clear";
        this->instr_info.is_reg_s = 0;
        this->instr_info.is_reg_d = 0;
        this->instr_info.arg_order = T;
    }
    if ( ( this->instr_info.instr == SLL)
         && ( this->reg_t.reg == RegFile::ZERO)
         && ( this->reg_d.reg == RegFile::ZERO) && ( this->imm == 0))
    {
        this->instr_info = ISA[ NOP];
    }
}

void FuncInstr::setDataOfRegs( uint64 data_src1, uint64 data_src2, uint64 data_dst)
{
    this->data_src1 = data_src1;
    this->data_src2 = data_src2;
    this->data_dst = data_dst;
};

uint64 FuncInstr::getImm()
{
    return this->imm;
}

FuncInstr::Instructions FuncInstr::getInstr()
{
    return this->instr_info.instr;
}

RegFile::Registers FuncInstr::getRegS()
{
    return this->reg_s.reg;
}

RegFile::Registers FuncInstr::getRegT()
{
    return this->reg_t.reg;
}

RegFile::Registers FuncInstr::getRegD()
{
    return this->reg_d.reg;
}


std::string FuncInstr::formDumpStr() const
{
    std::ostringstream oss;

    oss << this->instr_info.name_str << " ";
    
    switch ( this->instr_info.arg_order)
    {
        case DST:
            oss << this->reg_d.name_str << "(" << this->data_dst << "), "
                << this->reg_s.name_str << "(" << this->data_src1 << "), "
                << this->reg_t.name_str << "(" << this->data_src2 << ")";
            break;
        
        case TSI:
            oss << this->reg_t.name_str << "(" << this->data_dst << "), "
                << this->reg_s.name_str << "(" << this->data_src1 << "), "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case DTI:
            oss << this->reg_d.name_str << "(" << this->data_dst << "), "
                << this->reg_t.name_str << "(" << this->data_src1 << "), "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case STI:
            oss << this->reg_s.name_str << "(" << this->data_src1 << "), "
                << this->reg_t.name_str << "(" << this->data_src2 << "), "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case TS:
            oss << this->reg_t.name_str << "(" << this->data_dst << "), "
                << this->reg_s.name_str << "(" << this->data_src1 << ")";
            break;

        case DS:
            oss << this->reg_d.name_str << "(" << this->data_dst << "), "
                << this->reg_s.name_str << "(" << this->data_src1 << ")";
            break;

        case TI:
            oss << this->reg_t.name_str << "(" << this->data_dst << "), "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case I:
            oss << "0x" << std::hex << this->imm << std::dec;
            break;

        case S:
            oss << this->reg_s.name_str << "(" << this->data_src1 << ")";
            break;

        case T:
            oss << this->reg_t.name_str << "(" << this->data_dst << ")";
            break;

        case TIS:
            oss << this->reg_t.name_str << "(" << this->data_dst << "), "
                << ( sint64)this->imm << "(" 
                << this->reg_s.name_str << "(" << this->data_src1 << "))";
            break;

        case NOT:
            break;

    }

    //this->dump_str = oss.str();
    return oss.str();
}

std::string FuncInstr::dump( std::string indent) const
{
    std::string str = this->formDumpStr();
    std::ostringstream oss;

    //oss << indent << this->dump_str << std::endl;
    oss << indent << str << std::endl;

    return oss.str();
}

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.dump( "");
    return out;
}
