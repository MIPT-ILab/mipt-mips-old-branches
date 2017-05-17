// Generic C/C++
#include <iostream>
#include <cassert>
#include <sstream>
#include <assert.h>

// uArch sim modules
#include <elf_parser.h>
#include <types.h>
#include <func_instr.h>

//using namespace std;

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name   opcode    func         format          type
    { "add"  , 0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD},
    { "addu" , 0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD},
    { "sub"  , 0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::ADD},
    { "subu" , 0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::ADD},
    { "addi" , 0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD},
    { "addiu", 0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD},
    { "sll"  , 0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::SHT},
    { "srl"  , 0x0,     0x02, FuncInstr::FORMAT_R, FuncInstr::SHT},
    { "beq"  , 0x4,     0x00, FuncInstr::FORMAT_I, FuncInstr::COB},
    { "bne"  , 0x5,     0x00, FuncInstr::FORMAT_I, FuncInstr::COB},
    { "j"    , 0x2,     0x00, FuncInstr::FORMAT_J, FuncInstr::UCJ},
    { "jr"   , 0x0,     0x08, FuncInstr::FORMAT_R, FuncInstr::UCJ}
};

const string FuncInstr::regTable[] =
{
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9",
    "$k0", "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra"
};


FuncInstr::FuncInstr( uint32 bytes)
{
    this -> initFormat( bytes);
    switch ( this -> format)
    {
        case FORMAT_R :
            this -> parseR( bytes);
            break;
        case FORMAT_I :
            this -> parseI( bytes);
            break;
        case FORMAT_J :
            this -> parseJ( bytes);
            break;
        default :
            assert(0);
    }
}

void FuncInstr::initFormat( uint32 bytes)
{
    this->bytes.raw = bytes;
    //std::cout << this->bytes.raw << endl;
    uint32 opcode = this->bytes.asR.opcode;
    
    for( int i = 0; i < INSTR_NUM; i++)
    {
        if( opcode == this->isaTable[ i].opcode)
        {
            this->format = this->isaTable[ i].format;
            return;
        }
        //std::cout << i << endl;
    }

    // no opcode found
    std::cout << "WRONG INSTRUCTION OPCODE" << endl;
    assert(0);
}

void FuncInstr::parseR( uint32 bytes)
{
    ostringstream oss;
    if( this->bytes.raw == 0x0)
    {
        oss << "nop" << endl;
        this->str = oss.str();
        return;
    }
    if(( this->bytes.asR.func == 0x21) && ( this->bytes.asR.s == 0) && ( this->bytes.asR.d == 0))
    {
        oss << "clear " << this->regTable[ this->bytes.asR.t] << endl;
    }
    for( int i = 0; i < INSTR_NUM; i++)
    {
        if( this->bytes.asR.opcode == this->isaTable[ i].opcode)
        {
            if( this->bytes.asR.func == this->isaTable[ i].func)
            {
                oss << this->isaTable[ i].name << " ";
                if( this->isaTable[ i].type == ADD)
                {
                    oss << regTable[ this->bytes.asR.d] << ", "
                        << regTable[ this->bytes.asR.s] << ", "
                        << regTable[ this->bytes.asR.t] << std::endl;
                    this->str = oss.str();
                    return;
                }
                if( this->isaTable[ i].type == SHT)
                {
                    oss << regTable[ this->bytes.asR.d] << ", "
                        << regTable[ this->bytes.asR.t] << ", "
                        << this->bytes.asR.s << std::endl;
                    this->str = oss.str();
                    return;
                }
                if( this->isaTable[ i].type == UCJ)
                {
                    oss << regTable[ this->bytes.asR.s] << std::endl;
                    this->str = oss.str();
                    return;
                }
            }
        }
    }

    // cannot be parsed
    cout << "PARSING FAILED" << endl
         << "FUNCTION : parseR()" << endl;
    assert(0);
}

void FuncInstr::parseI( uint32 bytes)
{
    ostringstream oss;
    for( int i = 0; i < INSTR_NUM; i++)
    {
        if( this->bytes.asI.opcode == this->isaTable[ i].opcode)
        {
            if( this->isaTable[ i].type == ADD)
            {
                // check for move
                if( this->bytes.asI.opcode == 0x9 && this->bytes.asI.imm == 0x0)
                {
                    oss << "move " << this->regTable[ this->bytes.asI.t]
                        << this->regTable[ this->bytes.asI.s] << endl;
                    this->str = oss.str();
                    return;
                }
                // for ADD I type instructions
                oss << this->isaTable[ i].name << " "
                    << this->regTable[ this->bytes.asI.t] << ", "
                    << this->regTable[ this->bytes.asI.s] << ", "
                    << this->bytes.asI.imm << endl;
                this->str = oss.str();
                std::cout << this->str;
                return;
            }
            else
            {
                // for COB I type instructions
                oss << this->isaTable[ i].name << " "
                    << this->regTable[ this->bytes.asI.s] << ", "
                    << this->regTable[ this->bytes.asI.t] << ", "
                    << this->bytes.asI.imm << endl;
                this->str = oss.str();
                return;
            }
        }
    }

    // cannot be parsed
    cout << "PARSING FAILED" << endl
         << "FUNCTION : parseI()" << endl;
    assert(0);
}

void FuncInstr::parseJ( uint32 bytes)
{
    ostringstream oss;
    for( int i = 0; i < INSTR_NUM; i++)
    {
        if( this->bytes.asJ.opcode == this->isaTable[ i].opcode)
        {
            oss << this->isaTable[ i].name << " "
                << this->bytes.asJ.address << endl;
            this->str = oss.str();
            return;
        }
    }
    // cannot be parsed
    cout << "PARSING FAILED" << endl
         << "FUNCTION : parseJ()" << endl;
    assert(0);
}
