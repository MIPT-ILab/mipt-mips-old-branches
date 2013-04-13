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
    /*     name      opcode  funct    type  s  t  d im order*/
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
    {   LUI, "  lui",  0xF, UNUSED, I_TYPE, 0, 1, 0, 1,  TI}
};

const FuncInstr::RegName FuncInstr::RegNames[] =
{ 
    { ZERO, "$zero"}, { AT, "$at"}, { V0, "$v0"}, { V1, "$v1"},
    {   A0,   "$a0"}, { A1, "$a1"}, { A2, "$a2"}, { A3, "$a3"},
    {   T0,   "$t0"}, { T1, "$t1"}, { T2, "$t2"}, { T3, "$t3"}, 
    {   T4,   "$t4"}, { T5, "$t5"}, { T6, "$t6"}, { T7, "$t7"},
    {   S0,   "$s0"}, { S1, "$s1"}, { S2, "$s2"}, { S3, "$s3"}, 
    {   S4,   "$s4"}, { S5, "$s5"}, { S6, "$s6"}, { S7, "$s7"}, 
    {   T8,   "$t8"}, { T9, "$t9"}, { K0, "$k0"}, { K1, "$k1"}, 
    {   GP,   "$gp"}, { SP, "$sp"}, { FP, "$fp"}, { RA, "$ra"}
};


FuncInstr::FuncInstr( uint32 bytes)
{
    this->convertible.bytes = bytes;

    this->instr = selection();

    parseInstr();

    this->num_of_arguments = this->instr.is_reg_s + this->instr.is_reg_t +
                             this->instr.is_reg_d + this->instr.is_imm;

    formDumpStr();
}

FuncInstr::InstrInfo FuncInstr::selection() const
{

#if DEBUG        
    std::cout << "opcode: " << std::hex << this->convertible.asR.opcode 
              << std::dec << ", funct: " << std::hex
              << this->convertible.asR.funct << std::dec << std::endl;
#endif

    for ( int i = 0; i < SIZE_OF_ISA; i++)
    {
        if ( ( this->convertible.asR.opcode == ISA[ i].opcode) &&
             ( ( ISA[ i].funct == UNUSED) ||
               ( this->convertible.asR.funct == ISA[ i].funct)))
        {
            return ISA[ i];
        }
    }
    
    std::cerr << "ERROR: Instuction 0x" << std::hex << this->convertible.bytes
              << std::dec << " is not a MIPS instruction!" << std::endl;

    exit( EXIT_FAILURE);
}

void FuncInstr::parseInstr()
{
    // for all instructions position of rigesters like R_TYPE
    if ( this->instr.is_reg_s)
    {
        this->reg_s = RegNames[ this->convertible.asR.s];
    }
    if ( this->instr.is_reg_t)
    {
        this->reg_t = RegNames[ this->convertible.asR.t];
    }
    if ( this->instr.is_reg_d)
    {
        this->reg_d = RegNames[ this->convertible.asR.d];
    }
    if ( this->instr.is_imm)
    {
        switch ( this->instr.type)
        {
            case R_TYPE:
                this->imm = this->convertible.asR.shamt;
                break;
            case J_TYPE:
                this->imm = this->convertible.asJ.imm;
                break;
            case I_TYPE:
                this->imm = this->convertible.asI.imm;
                break;
        }
    }

    // pseudo-instructions
    if ( ( this->instr.name == ADDIU) && ( this->imm == 0))
    {
        this->instr.name_str = " move";
        this->instr.is_imm = 0;
        this->instr.arg_order = TS;
    }
    if ( ( this->instr.name == ADDU) && ( this->reg_t.reg == ZERO))
    {
        this->instr.name_str = " move";
        this->instr.is_reg_t = 0;
        this->instr.arg_order = TS;
    }
    if ( ( this->instr.name == ADDU) && ( this->reg_s.reg == ZERO) &&
         ( this->reg_d.reg == ZERO))
    {
        this->instr.name_str = "clear";
        this->instr.is_reg_s = 0;
        this->instr.is_reg_d = 0;
        this->instr.arg_order = T;
    }
    if ( ( this->instr.name == SLL) && ( this->reg_t.reg == ZERO) &&
         ( this->reg_d.reg == ZERO) && ( this->imm == 0))
    {
        this->instr.name_str = "  nop";
        this->instr.is_reg_t = 0;
        this->instr.is_reg_d = 0;
        this->instr.is_imm = 0;
        this->instr.arg_order = NOT;
    }
}

void FuncInstr::formDumpStr()
{
    std::ostringstream oss;

    oss << this->instr.name_str << " ";
    
    switch ( this->instr.arg_order)
    {
        case DST:
            oss << this->reg_d.name_str << ", " << this->reg_s.name_str << ", "
                << this->reg_t.name_str;
            break;
        
        case TSI:
            oss << this->reg_t.name_str << ", " << this->reg_s.name_str << ", "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case DTI:
            oss << this->reg_d.name_str << ", " << this->reg_t.name_str << ", "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case STI:
            oss << this->reg_s.name_str << ", " << this->reg_t.name_str << ", "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case TS:
            oss << this->reg_t.name_str << ", " << this->reg_s.name_str;
            break;

        case I:
            oss << "0x" << std::hex << this->imm << std::dec;
            break;

        case S:
            oss << this->reg_s.name_str;
            break;

        case T:
            oss << this->reg_t.name_str;
            break;

        case TIS:
            oss << this->reg_t.name_str << ", " << this->imm 
                << "(" << this->reg_s.name_str << ")";
            break;

        case TI:
            oss << this->reg_t.name_str << ", "
                << "0x" << std::hex << this->imm << std::dec;
            break;

        case NOT:
            break;

    }

    this->dump_str = oss.str();
}

std::string FuncInstr::dump( std::string indent) const
{
    std::ostringstream oss;

    oss << indent << this->dump_str << std::endl;

    return oss.str();
}

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.dump( "");
    return out;
}
