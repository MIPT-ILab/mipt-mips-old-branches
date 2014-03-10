// Generic C

// Generic C++
#include <iostream>
#include <iomanip>
// uArchSim modules
#include <func_instr.h>

using namespace std;
//functs of I and J-type are 0xF in the table, since they don't matter
const FuncInstr::ISAEntry FuncInstr::isaTable[] = 
{
    { "add", 0x0, 0x20, FuncInstr::FORMAT_R, FuncInstr::ADD }, // 0
    { "addu", 0x0, 0x21, FuncInstr::FORMAT_R, FuncInstr::ADD },// 1
    { "sub", 0x0, 0x22, FuncInstr::FORMAT_R, FuncInstr::ADD }, // 2
    { "subu", 0x0, 0x23, FuncInstr::FORMAT_R, FuncInstr::ADD },// 3
    { "addi", 0x8, 0xF, FuncInstr::FORMAT_I, FuncInstr::ADD },    // 4
    { "addiu", 0x9, 0xF, FuncInstr::FORMAT_I, FuncInstr::ADD },   // 5
    { "sll", 0x0, 0x0, FuncInstr::FORMAT_R, FuncInstr::SHIFT },// 6
    { "srl", 0x0, 0x2, FuncInstr::FORMAT_R, FuncInstr::SHIFT },// 7
    { "beq", 0x4, 0xF, FuncInstr::FORMAT_I, FuncInstr::BRANCH },  // 8
    { "bne", 0x5, 0xF, FuncInstr::FORMAT_I, FuncInstr::BRANCH },  // 9
    { "j", 0x2, 0xF, FuncInstr::FORMAT_J, FuncInstr::JUMP },      // 10
    { "jr", 0x0, 0x8, FuncInstr::FORMAT_R, FuncInstr::JUMP }   // 11
};

const string FuncInstr::regString[] =
{ 
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",  
    "$t8", "$t9", "$k0","$k1", "$gp", "$sp", "$fp", "$ra"
};

const int8 FuncInstr::tablesize = 12;

FuncInstr::FuncInstr( uint32 bytes)
{
    this->initFormat(bytes);
    switch (this->format)
    {
        case FORMAT_R:
            this->parseR();
            break;
        case FORMAT_I:
            this->parseI();
            break;
        case FORMAT_J:
            this->parseJ();
            break;
        default:
            assert(0);
    }
};

void FuncInstr::initFormat( uint32 bytes)
{
    //This function relies heavily on the current isaTable
    //(Since the general rule to determine the format is unknown)
    //and may need to be rewritten if we add an instruction to it
    this->bytes.raw = bytes;
    unsigned opcode = this->bytes.asI.opcode;
    if ( opcode)
    {
        if ( opcode == 2)
        {
            this->format = FORMAT_J;
        }
        else
        {
            unsigned shifted = opcode >> 2;
            bool Opcode_exists_in_ISA_Table = ( shifted == 1) || ( shifted == 2);
            assert (Opcode_exists_in_ISA_Table);
            this->format = FORMAT_I;
        }
    }
    else
    {
        this->format = FORMAT_R;
    }
    return;
};

void FuncInstr::parseR()
{
    instr.numb = -1;
    for ( int8 i = 0; i < tablesize; i++)
    {
        if ( isaTable[i].funct == bytes.asR.funct)
        {
            instr.numb = i;
            break;
        }
    }
    //Asserting instruction is used in the project (is in ISA Table)
    assert( instr.numb != -1);
    
    instr.regs =  bytes.asR.s;
    instr.regt =  bytes.asR.t;
    instr.regd =  bytes.asR.d;
    instr.shamt = ( uint8) bytes.asR.shamt;
    return;
};

void FuncInstr::parseI()
{
    instr.numb = -1;
    for ( int8 i = 0; i < tablesize; i++)
    {
        if ( isaTable[i].opcode == bytes.asI.opcode)
        {
            instr.numb = i;
            break;
        }
    }
    assert( instr.numb != -1);
    
    instr.regs = bytes.asI.s; 
    instr.regt = bytes.asI.t;
    instr.imm = bytes.asI.imm;
    return;
};

void FuncInstr::parseJ()
{
    instr.numb = -1;
    //Excessive, since instr.numb == 10, but this way it doesn't rely this much on current isaTable
    for ( int8 i = 0; i < tablesize; i++)
    {
        if ( isaTable[i].opcode == bytes.asJ.opcode)
        {
            instr.numb = i;
            break;
        }
    }
    assert( instr.numb != -1);
    instr.addr = bytes.asJ.addr;
    return;
};

string FuncInstr::Dump( string indent) const
{
    ISAEntry info = isaTable[ instr.numb];
    ostringstream out;
    ostringstream pseudo;
    bool pseudoflag = false;
    out << hex << indent << info.name << ' ';
    pseudo << hex << indent;
    switch (  info.format)
    {
        case FORMAT_R:
            switch ( info.type)
            {
                case ADD:
                    if(( info.funct == 0x21) && ( instr.regs == 0) && ( instr.regd == 0))
                    {
                        pseudo << "clear " << regString[ instr.regt] << endl;
                        pseudoflag = true;
                    }
                    out << regString[ instr.regd] << ", " << regString[ instr.regs] << ", "
                    << regString[ instr.regt] << endl;
                    break;
                case SHIFT:
                    if( bytes.raw == 0)
                    {
                        pseudo << "nop" << endl;
                        pseudoflag = true;
                    }
                    out << regString[ instr.regd] << ", " << regString[ instr.regt] << ", "
                    << "0x" << ( unsigned) instr.shamt << endl;
                    break;
                case JUMP:
                    out << regString[ instr.regs] << endl;
                    break;
                default: assert(0);
            }
            break;
        case FORMAT_I:
            switch ( info.type)
            {
                case ADD:
                    if(( info.opcode == 0x9) && (instr.imm == 0))
                    {
                        pseudo << "move " << regString[ instr.regt] << ", " << regString[ instr.regs]
                        << ", 0" <<  endl;
                        // definition was taken from the project wiki
                        // (definition in objdump -D seems to differ from this, it uses addu)
                        pseudoflag = true;
                    }
                    out << regString[ instr.regt] << ", " << regString[ instr.regs] << ", "
                    << "0x" << ( unsigned) instr.imm << endl;
                    break;
                case BRANCH:
                    out << regString[ instr.regs] << ", " << regString[ instr.regt] << ", "
                    << "0x" << ( unsigned) instr.imm << endl;
                    break;
                default: assert(0);
            }
            break;
        case FORMAT_J:
            out << "0x" << ( unsigned) instr.addr << endl;
            break;
        default: assert(0);
    }
    if ( pseudoflag)
    {
        return pseudo.str();
    }
    else
    {
        return out.str();
    }
}

ostream& operator<< ( ostream& out, const FuncInstr& instr)
{
     return (out << instr.Dump(""));
}
