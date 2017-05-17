#include <func_instr.h>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name   opcode     func      format              type
    { "add",    0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD   /*...*/ },
    { "addu",   0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU  /*...*/ },
    { "sub",    0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB   /*...*/ },
    { "subu",   0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU  /*...*/ },
    { "addi",   0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADDI  /*...*/ },
    { "addiu",  0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADDIU /*...*/ },
    { "srl",    0x0,     0x02, FuncInstr::FORMAT_R, FuncInstr::SRL   /*...*/ },
    { "beq",    0x4,     0x00, FuncInstr::FORMAT_I, FuncInstr::BEQ   /*...*/ },
    { "bne",    0x5,     0x00, FuncInstr::FORMAT_I, FuncInstr::BNE   /*...*/ },
    { "j",      0x2,     0x20, FuncInstr::FORMAT_J, FuncInstr::J     /*...*/ },
    { "jr",     0x0,     0x08, FuncInstr::FORMAT_R, FuncInstr::JR    /*...*/ },
    { "sll",    0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::SLL   /*...*/ },
    // more instructions ...
    { ""      , 0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::ISAEND/*...*/ },
};

const char* FuncInstr::regTable[] =
{
    // name   address
    "$zero", //0x00
    "$at", //0x01
    "$v0", //0x02
    "$v1", //0x03
    "$a0", //0x04
    "$a1", //0x05
    "$a2", //0x06
    "$a3", //0x07
    "$t0", //0x08
    "$t1", //0x09
    "$t2", //0x0A
    "$t3", //0x0B
    "$t4", //0x0C
    "$t5", //0x0D
    "$t6", //0x0E
    "$t7", //0x0F
    "$s0", //0x10
    "$s1", //0x11
    "$s2", //0x12
    "$s3", //0x13
    "$s4", //0x14
    "$s5", //0x15
    "$s6", //0x16
    "$s7", //0x17
    "$t8", //0x18
    "$t9", //0x19
    "$k0", //0x1A
    "$k1", //0x1B
    "$gp", //0x1C
    "$sp", //0x1D
    "$fp", //0x1E
    "$ra"  //0x1F
};

FuncInstr::FuncInstr( uint32 bytes)
{
    this->bytes.raw = bytes;
    initFormat();
    switch (format)
    {
        case FORMAT_R:
            parseR();
            break;
        case FORMAT_I:
            parseI();
            break;
        case FORMAT_J:
            parseJ();
            break;
        default:
            assert(0);
    }
};

int FuncInstr::parseR(){

    for(int i = 0; isaTable[i].type!=ISAEND; i++)
                if(bytes.asR.opcode == isaTable[i].opcode &&
                    bytes.asR.func == isaTable[i].func)
                {
                    parsed << isaTable[i].name << " "
                           << regTable[bytes.asR.d] << ", "
                           << regTable[bytes.asR.s] << ", "
                           << regTable[bytes.asR.t];
                    return 1;
                }
    std::cerr << "ERROR: unknown instruction " << std::endl;
    exit( EXIT_FAILURE);
}

int FuncInstr::parseI(){

    for(int i = 0; isaTable[i].type!=ISAEND; i++)
                if(bytes.asI.opcode == isaTable[i].opcode)
                {
                    parsed << isaTable[i].name << " "
                           << regTable[bytes.asI.t] << ", "
                           << regTable[bytes.asI.s] << ", "
                           << std::hex << "0x" << bytes.asI.imm;
                    return 1;
                }
    std::cerr << "ERROR: unknown instruction " << std::endl;
    exit( EXIT_FAILURE);
}

int FuncInstr::parseJ(){

    for(int i = 0; isaTable[i].type!=ISAEND; i++)
                if(bytes.asJ.opcode == isaTable[i].opcode)
                {
                    parsed << isaTable[i].name << " "
                           << std::hex << "0x" << bytes.asJ.addr;
                    return 1;
                }
    std::cerr << "ERROR: unknown instruction " << std::endl;
    exit( EXIT_FAILURE);
}

int FuncInstr::initFormat()
{
    for(int i = 0; isaTable[i].type!=ISAEND; i++)
        if(bytes.asDefault.opcode == isaTable[i].opcode)
        {
            format = isaTable[i].format;
            return 0;
        }
    std::cerr << "ERROR: unknown instruction " << std::endl;
    exit( EXIT_FAILURE);
}

std::string FuncInstr::Dump( std::string indent) const
{
    return parsed.str();
}

  //  this->tReg = this->bytes.asI.t;
// ...

//::ostream& operator<<( std::ostream& out, const FuncInstr& instr);
