/**
 * func_instr.cpp - Implementation of the instruction decoder 
 * @author Alexander Kravtcov
 */

// Generic C
#include <cerrno>
#include <cstdlib>

// Generic C++
#include <string>
#include <iostream>
#include <sstream>

// uArchSim modules
#include <func_instr.h>

FuncInstr::FuncInstr( uint32 bytes)
{
    this->convertible.bytes = bytes;

    this->instr = selection();

    parseInstr();

    this->num_of_arguments = this->instr.is_reg_s + this->instr.is_reg_t +
                             this->instr.is_reg_d + this->instr.is_imm;
}

InstrInfo FuncInstr::selection() const
{
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
    if ( ( this->instr.name == "addiu") && ( this->imm == 0))
    {
        this->instr.name = "move";
        this->instr.is_imm = 0;
    }
    if ( ( this->instr.name == "addu") && ( this->reg_t == "$zero"))
    {
        this->instr.name = "move";
        this->instr.is_reg_t = 0;
    }
    if ( ( this->instr.name == "addu") && ( this->reg_s == "$zero") &&
         ( this->reg_d == "$zero"))
    {
        this->instr.name = "clear";
        this->instr.is_reg_s = 0;
        this->instr.is_reg_d = 0;
    }
    if ( ( this->instr.name == "sll") && ( this->reg_t == "$zero") &&
         ( this->reg_d == "$zero") && ( this->imm == 0))
    {
        this->instr.name = "nop";
        this->instr.is_reg_t = 0;
        this->instr.is_reg_d = 0;
        this->instr.is_imm = 0;
    }
}

std::string FuncInstr::dump( std::string indent) const
{
    std::ostringstream oss;

    oss << indent << this->instr.name << " ";
    
    int printed_num_of_arguments = 0;

    if ( this->instr.is_reg_d)
    {
        oss << this->reg_d;
        if ( ++printed_num_of_arguments < this->num_of_arguments)
        {
            oss << ", ";
        }
    }
    
    // for this instructions ( "addi" and "addiu") register t must be before s
    if ( !this->instr.is_right_order_reg)
    {
        if ( this->instr.is_reg_t)
        {
            oss << this->reg_t;
            if ( ++printed_num_of_arguments < this->num_of_arguments)
            {
                oss << ", ";
            }
        }
        if ( this->instr.is_reg_s)
        {
            oss << this->reg_s;
            if ( ++printed_num_of_arguments < this->num_of_arguments)
            {
                oss << ", ";
            }
        }
    } else
    {
        if ( this->instr.is_reg_s)
        {
            oss << this->reg_s;
            if ( ++printed_num_of_arguments < this->num_of_arguments)
            {
                oss << ", ";
            }
        }
        if ( this->instr.is_reg_t)
        {
            oss << this->reg_t;
            if ( ++printed_num_of_arguments < this->num_of_arguments)
            {
                oss << ", ";
            }
        }
    }

    if ( this->instr.is_imm)
    {
        oss << "0x" << std::hex << this->imm << std::dec;
    }

    oss << std::endl;

    return oss.str();
}

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.dump( "");
    return out;
}
