/** 
 * func_instr.cpp - realization for methods of FuncInstr class
 * @author Dmitry Ermilov <ermilov@phystech.edu>
 * Copyright 2012 uArchSim iLab project, 2012 uArchSim iLab project
 */

// Genereic C
#include <cassert>
#include <cstdlib> 

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_instr.h>
#include <types.h>

using namespace std;

//Initialization of table which contain information about commands 
const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name     opcode    func      format              type
    { "add ",   0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD },
    { "addu ",  0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU },
    { "sub ",   0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB },
    { "subu ",  0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU },
    { "addi ",  0x8,     0x0,  FuncInstr::FORMAT_I, FuncInstr::ADDI },
    { "addiu ", 0x9,     0x0,  FuncInstr::FORMAT_I, FuncInstr::ADDIU },
    { "sll ",   0x0,     0x0,  FuncInstr::FORMAT_R, FuncInstr::SLL },
    { "srl ",   0x0,     0x2,  FuncInstr::FORMAT_R, FuncInstr::SRL },
    { "beq ",   0x4,     0x0,  FuncInstr::FORMAT_I, FuncInstr::BEQ },
    { "bne ",   0x5,     0x0,  FuncInstr::FORMAT_I, FuncInstr::BNE },
    { "j ",     0x2,     0x0,  FuncInstr::FORMAT_J, FuncInstr::J },
    { "jr ",    0x0,     0x8,  FuncInstr::FORMAT_R, FuncInstr::JR }
};

//Initialization of table which contain information about registers 
const FuncInstr::REGEntry FuncInstr::regTable[] = 
{
    { "$zero", FuncInstr::ZERO },
    { "$at",   FuncInstr::AT },
    { "$t0",   FuncInstr::T0 },
    { "$t1",   FuncInstr::T1 },
    { "$t2",   FuncInstr::T2 },
    { "$t3",   FuncInstr::T3 },
    { "$t4",   FuncInstr::T4 },
    { "$t5",   FuncInstr::T5 },
    { "$t6",   FuncInstr::T6 },
    { "$t7",   FuncInstr::T7 },
    { "$t8",   FuncInstr::T8 },
    { "$t9",   FuncInstr::T9 }
};

//It's constructer with 1 parametre
FuncInstr::FuncInstr( uint64 bytes_)
{
    if ( bytes_ < ( 1ul << 32) && bytes_)
    {
        this->initFormat( (uint32) bytes_);
        switch (this->format)
        {
            case FORMAT_R:
                this->parseR( (uint32) bytes_);
                break;
            case FORMAT_I:
                this->parseI( (uint32) bytes_);
                break;
            case FORMAT_J:
                this->parseJ( (uint32) bytes_);
                break;
            default:
                assert(0);
        }
        this->tab_isa_size = 12;
        this->tab_reg_size = 12;
    }
    else  if ( bytes_ != 0)
    {
        cerr << "ERROR.*" << endl;
        exit( EXIT_FAILURE);
    }
};

//This method defines format of operation
void FuncInstr::initFormat( uint32 bytes_)
{
    this->bytes.raw = bytes_;
    unsigned form = bytes.asR.opcode;
    if ( form == 0)
        this->format = FORMAT_R;
    else  if ( form == 2)
        this->format = FORMAT_J;
    else  if ( form == 4 || form == 5 || form == 8 || form == 9)
        this->format = FORMAT_I;
    else 
        assert(0);
}

//It's parser for R-commands. It writes command number and registers
void FuncInstr::parseR( uint32 bytes_)
{
    this->bytes.raw = bytes_;
    char cflag = check_typeR( ( enum Type) this->bytes.asR.funct);
    assert(cflag);
    this->type = ( enum Type) this->bytes.asR.funct;
    put_reg( this->bytes.asR.d, this->dest);
    put_reg( this->bytes.asR.s, this->source);
    put_reg( this->bytes.asR.t, this->target);
}

//It's parser for R-commands. It writes command number, registers and immediate
void FuncInstr::parseI( uint32 bytes_)
{
    this->bytes.raw = bytes_;
    char cflag = check_typeI( ( enum Type) this->bytes.asI.opcode);
    assert(cflag);
    this->type = ( enum Type) this->bytes.asI.opcode;
    put_reg( this->bytes.asI.t, this->dest);
    put_reg( this->bytes.asI.s, this->source);
    this->immediate = this->bytes.asI.imm;
}

//It's parser for R-commands. It writes command and adress
void FuncInstr::parseJ( uint32 bytes_)
{
    this->bytes.raw = bytes_;
    char cflag = check_typeJ( ( enum Type) this->bytes.asJ.opcode);
    assert(cflag);
    this->type = ( enum Type) this->bytes.asJ.opcode;
    this->address = this->bytes.asJ.address;
}

//This method puts value for "field" (register).
void FuncInstr::put_reg( int reg, enum Registers &field)
{
    char cflag = check_reg( ( enum Registers) reg);
    assert(cflag);
    field = ( enum Registers) reg;
}

//This method proofs if the register is correct
bool FuncInstr::check_reg( enum Registers reg) const
{
    return ( reg == ZERO || reg == AT || reg >= T0 && reg <= T7 
          || reg == T8 || reg == T9);
}

//This method proofs if the command refers to R-type
bool FuncInstr::check_typeR( enum Type type) const
{
    return ( type >= ADD && type <= SUBU || type == SLL || type == SRL 
          || type == JR);
}

//This method proofs if the command refers to I-type
bool FuncInstr::check_typeI( enum Type type) const
{
    return ( type == ADDI || type == ADDIU || type == BEQ || type == BNE );
}

//This method proofs if the command refers to J-type
bool FuncInstr::check_typeJ( enum Type type) const
{
    return ( type == J);
}

//This method chooses right dump function and returns string for output
std::string FuncInstr::Dump( std::string indent) const
{
    std::string str( indent);
    switch (this->format)
    {
        case FORMAT_R:
            this->dumpR(str);
            break;
        case FORMAT_I:
            this->dumpI(str);
            break;
        case FORMAT_J:
            this->dumpJ(str);
            break;
        default:
            assert(0);
    }
    return str;
}

//This method makes string for R-format
void FuncInstr::dumpR( std::string &str) const
{
    int i = get_isa_num();
    str += isaTable[i].name;
    str += regTable[get_reg_num( this->dest)].name;
    str += ", ";
    str += regTable[get_reg_num( this->source)].name;
    str += ", ";
    str += regTable[get_reg_num( this->target)].name;
}

//This method makes string for I-format
void FuncInstr::dumpI( std::string &str) const
{
    int i = get_isa_num();
    str += isaTable[i].name;
    str += regTable[get_reg_num( this->dest)].name;
    str += ", ";
    str += regTable[get_reg_num( this->source)].name;
    str += ", ";
    str += "0x";
    std::ostringstream ost;
    ost << hex << this->immediate << dec;
    str += ost.str();
}

//This method makes string for J-format
void FuncInstr::dumpJ( std::string &str) const
{
    int i = get_isa_num();
    str += isaTable[i].name;
    str += "0x";
    std::ostringstream ost;
    ost << hex << this->address << dec;
    str += ost.str();
}

//This method finds in isaTable the number of command with necessary format and type
int FuncInstr::get_isa_num() const
{
    uint32 i;
    for ( i = 0; 
          ( this->isaTable[i].type != this->type || 
            this->isaTable[i].format != this->format) &&
            i < this->tab_isa_size;
          i++);
    cout << endl;
    assert( i != tab_isa_size);
    return i;
}

//This method finds in regTable number of register which we need to add to a string
int FuncInstr::get_reg_num( enum Registers reg_) const
{
    uint32 i;
    for ( i = 0; this->regTable[i].reg != reg_ && i < this->tab_reg_size; i++);
    assert( i != this->tab_reg_size);
    return i;
}

//This method puts out the disassembled command
std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    
    return out << instr.Dump("    ") << endl;
}
