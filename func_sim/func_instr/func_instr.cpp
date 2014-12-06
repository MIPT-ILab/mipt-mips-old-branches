
#include <func_instr.h>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>


struct ISAEntry
{
    const char* name;

    uint8 opcode;
    uint8 func;

    FuncInstr::Format format;
    FuncInstr::Type type;
};

const FuncInstr::ISAEntry[] FuncInstr::isaTable =
{
    // name   opcode     func      format              type
    { "add",    0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD    },
    { "addu",   0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD    },
    { "sub",    0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB    },
    { "subu",   0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUB    },
    { "addi",   0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD    },
    { "addiu",  0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD    },
    { "sll",    0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::SLL    },
    { "srl",    0x0,     0x02, FuncInstr::FORMAT_R, FuncInstr::SRL    },
    { "beq",    0x4,     0x00, FuncInstr::FORMAT_I, FuncInstr::BRANCH },
    { "bne",    0x5,     0x00, FuncInstr::FORMAT_I, FuncInstr::BRANCH },
    { "j",      0x2,     0x20, FuncInstr::FORMAT_J, FuncInstr::JUMP   },
    { "jr",     0x0,     0x08, FuncInstr::FORMAT_R, FuncInstr::JUMP   },

};

const char* FuncInstr::regTable[] =
{
    // name
    "$zero",
    "$at",
    "$t0",
    "$t1",
    "$t2",
    "$t3",
    "$t4",
    "$t5",
    "$t6",
    "$t7",
    "$t8",
    "$t9",
    "$gp",

};


void FuncInstr::FuncInstr( uint32 bytes)
{
    this->initFormat(bytes);
    switch (this->format)
    {
        case FORMAT_R:
            this->parser(bytes);
            break;
        case FORMAT_I:
            this->parseI(bytes);
            break;
        case FORMAT_J:
            this->parseJ(bytes);
            break;
        default:
            assert(0);
    }
};

int FuncInstr::parseR(){

    this->bytes.raw = bytes;
    this->dReg = this->bytes.asR.d;
    this->tReg = this->bytes.asR.t;
    this->sReg = this->bytes.asR.s;
    this->imm  = this->bytes.asR.S;
}

int FuncInstr::parseI(){

    this->bytes.raw = bytes;
    this->tReg = this->bytes.asI.t;
    this->sReg = this->bytes.asI.s;
    this->imm  = this->bytes.asI.imm;
}

int FuncInstr::parseJ(){

    this->bytes.raw = bytes;
    this->addr = this->bytes.asJ.address;
}

int FuncInstr::initFormat(unit32 bytes)
{
int format = bytes >> (26);
    if(format == 0x0)
    {
        this->format = FORMAT_R;
    }
    else if(format == 0x2)
    {
        this->format = FORMAT_J;
    }
    else
    {
        this->format = FORMAT_I;
    }
    cout << hex <<format << dec <<endl;
}
string FuncInstr::getREGname(const uint8 reg) const
{   if(reg == 28)
        return string("$gp");
    if(reg == 29)
        return string("$sp");
    if(reg == 30)
        return string("$fp");
    if(reg == 31)
        return string("$ra");
    assert("unknown register");
    return NULL;
    if(reg == 0)
        return string("$zero");
    if(reg == 1)
        return string("$at");
}

std::string FuncInstr::Dump( std::string indent) const
{
    switch (this->format)
    {
        case FORMAT_R:
            instrId = getISAFromTable(this->bytes.asR.opcode, this->bytes.asR.funct);
            switch(isaTable[instrId].type) {
                case FuncInstr::ADD:
                case FuncInstr::SUB:
                 parsed << getISAname(instrId) << " "
                        << getREGname(this->bytes.asR.d) << ","
                        << getREGname(this->bytes.asR.s) << ","
                        << getREGname(this->bytes.asR.t) << endl;
                    break;
                case FuncInstr::SHIFT:
                 parsed << getISAname(instrId) << " "
                        << getREGname(this->bytes.asR.d) << ","
                        << getREGname(this->bytes.asR.t) << ","
                        << getREGname(this->bytes.asR.S) << endl;
                    break;
                case FuncInstr::JUMP:
                 parsed << getISAname(instrId) << " "
                        << getREGname(this->bytes.asR.s) << endl;
                    break;
                default:
                    assert("unknown instruction");
            }
            break;
        case FORMAT_I:
            instrId = getISAFromTable(this->bytes.asI.opcode);
            switch(isaTable[instrId].type) {
                case ADD:
                 parsed << getISAname(instrId) << " "
                        << getREGname(this->bytes.asI.t) << ", "
                        << getREGname(this->bytes.asI.s) << ", "
                        << this->bytes.asI.imm << endl;
                    break;
                case BRANCH:
                 parsed << getISAname(instrId) << " "
                        << getREGname(this->bytes.asI.s) << ", "
                        << getREGname(this->bytes.asI.t) << ", "
                        << this->bytes.asI.imm << endl;
                    break;
            }
            break;

    return parsed.str();
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{

    return out << instr.Dump("");
}
