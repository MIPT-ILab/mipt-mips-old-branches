/*
 * func_instr.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: thar0l
 */

//
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_instr.h>

const FuncInstr::ISAEntry FuncInstr::isaTable[INSTRUCTIONS_COUNT] =
{
    //name     opcode   funct format               type
    { "add",   0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::MATH },
    { "addu",  0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::MATH },
    { "sub",   0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::MATH },
    { "subu",  0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::MATH },
    { "addi",  0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::MATH },
    { "addiu", 0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::MATH },
    { "sll",   0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::SHIFT },
    { "srl",   0x0,     0x02, FuncInstr::FORMAT_R, FuncInstr::SHIFT },
    { "beq",   0x4,     0x00, FuncInstr::FORMAT_I, FuncInstr::C_BRANCH },
    { "bne",   0x5,     0x00, FuncInstr::FORMAT_I, FuncInstr::C_BRANCH },
    { "j",     0x2,     0x00, FuncInstr::FORMAT_J, FuncInstr::UC_BRANCH },
    { "jr",    0x0,     0x08, FuncInstr::FORMAT_R, FuncInstr::UC_BRANCH }
};

const std::string FuncInstr::piTable[PSEUDO_INSTRUCTIONS_COUNT] =
{
        "move",
        "clear",
        "nop"
};

const std::string FuncInstr::regTable[REGISTERS_COUNT] =
{
        "$zero",
        "$at",
        "$v0", "$v1",
        "$a0", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9",
        "$k0", "$k1",
        "$gp",
        "$sp",
        "$fp",
        "$ra"
};

const int FAIL = 0;

FuncInstr::FuncInstr( uint32 bytes)
{
    format=FORMAT_R;
    this->initFormat(bytes);
    switch (this->format)
    {
        case FORMAT_R:
            this->parseR(bytes);
            break;
        case FORMAT_I:
            this->parseI(bytes);
            break;
        case FORMAT_J:
            this->parseR(bytes);
            break;
        default:
            assert("FuncInstr construction" == FAIL);
    }
};


void FuncInstr::initFormat( uint32 bytes)
{
    this->bytes.raw = bytes;
    uint32 opcode = this->bytes.asR.opcode;

    for (int i = 0; i < INSTRUCTIONS_COUNT; i++)
    {
        if (this->isaTable[i].opcode == opcode)
        {
            this->format = this->isaTable[i].format;
            return;
        }
    }

    std::cout<<"Unknown command: opcode="<<opcode<<std::endl;
    assert("InitFormat" == FAIL);
}

void FuncInstr::parseR( uint32 bytes)
{
    for (int i = 0; i < INSTRUCTIONS_COUNT; i++)
    {
        if ((this->isaTable[i].opcode == this->bytes.asR.opcode)&&(this->isaTable[i].func == this->bytes.asR.funct))
        {
            std::ostringstream oss;
            if((this->bytes.asR.opcode==0x0)&&(this->bytes.asR.funct==0x21)&&(this->bytes.asR.rs == 0)&&(this->bytes.asR.rt == 0))
            {
                    oss << this->piTable[1] << " "; //clear
                    oss << this->regTable[this->bytes.asR.rd];
            }
            else if(this->bytes.raw == 0x0)
            {
                    oss << this->piTable[2] << " "; //clear
            }
            else
            {
                oss << this->isaTable[i].name << " ";
                if (this->isaTable[i].type==UC_BRANCH)
                    {
                        oss << this->regTable[this->bytes.asR.rs];
                    }
                if (this->isaTable[i].type==MATH)
                    {
                        oss << this->regTable[this->bytes.asR.rd]<<", ";
                        oss << this->regTable[this->bytes.asR.rs]<<", ";
                        oss << this->regTable[this->bytes.asR.rt];
                    }
                if (this->isaTable[i].type==SHIFT)
                    {
                        oss << this->regTable[this->bytes.asR.rd]<<", ";
                        oss << this->regTable[this->bytes.asR.rs]<<", ";
                        oss << this->bytes.asR.shamt;
                    }
            }
            oss << std::endl;
            this->dump_str = oss.str();
            return;
        }
    }

    std::cout<<"Unknown command: opcode="<<this->bytes.asR.opcode<<" funct="<<this->bytes.asR.funct<<std::endl;
    assert(0);
}

void FuncInstr::parseI( uint32 bytes)
{
    for (int i = 0; i < INSTRUCTIONS_COUNT; i++)
    {
        if (this->isaTable[i].opcode == this->bytes.asR.opcode)
        {
            std::ostringstream oss;

            if (this->bytes.asI.imm == 0)
            {
                    oss << this->piTable[0] << " ";    //move
                    oss << this->regTable[this->bytes.asI.rs]<<", ";
                    oss << this->regTable[this->bytes.asI.rt];
            }
            else
            {
                oss << this->isaTable[i].name << " ";
                oss << this->regTable[this->bytes.asI.rs]<<", ";
                oss << this->regTable[this->bytes.asI.rt]<<", ";
                oss << this->bytes.asI.imm;
            }
            oss << std::endl;
            this->dump_str = oss.str();
            return;
        }
    }

    std::cout<<"Unknown command: opcode="<<this->bytes.asR.opcode<<std::endl;
    assert(0);
}

void FuncInstr::parseJ( uint32 bytes)
{
    for (int i = 0; i < INSTRUCTIONS_COUNT; i++)
    {
        if (this->isaTable[i].opcode == this->bytes.asR.opcode)
        {
            std::ostringstream oss;
            oss << this->isaTable[i].name << " ";
            oss << this->regTable[this->bytes.asJ.offset];
            oss << std::endl;
            this->dump_str = oss.str();
            return;
        }
    }

    std::cout<<"Unknown command: opcode="<<this->bytes.asR.opcode<<std::endl;
    assert(0);
}

