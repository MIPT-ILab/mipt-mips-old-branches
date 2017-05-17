//Generic C++
#include <iostream>

//Generic C
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include <cerrno>
#include <cassert>

//uArch sim modules
#include <func_instr.h>

//my modules
#include <my_assert.h>



//Constructor

FuncInstr::FuncInstr( uint32 bytes)
{
    this->initFormat(bytes);
    switch (this->format)
    {
        case FORMAT_R:
            this->initR(bytes);
            break;
        case FORMAT_I:
            this->initI(bytes);
            break;
        case FORMAT_J:
            this->initJ(bytes);
            break;
        default:
            MY_ASSERT(NULL, "Unknown format of instruction");
    }
}

//MIPS Instruction table

const FuncInstr::ISAEntry FuncInstr::isaTable[NUM_OF_COMMANDS] =
{
    // name   opcode    func      format                    type

    //Add and substruct

    { "add",   0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD_OR_SUB },
    { "addu",  0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD_OR_SUB },
    { "sub",   0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::ADD_OR_SUB },
    { "subu",  0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::ADD_OR_SUB },
    { "addi",  0x8,     0x00,  FuncInstr::FORMAT_I, FuncInstr::ADD_OR_SUB },
    { "addiu", 0x9,     0x00,  FuncInstr::FORMAT_I, FuncInstr::ADD_OR_SUB },

    //Shifts

    { "sll",   0x0,     0x00,  FuncInstr::FORMAT_R, FuncInstr::SHIFT },
    { "srl",   0x0,     0x02,  FuncInstr::FORMAT_R, FuncInstr::SHIFT },

    //Conditional branches

    { "beq",   0x4,     0x00,  FuncInstr::FORMAT_I, FuncInstr::COND_BRANCH },
    { "bne",   0x5,     0x00,  FuncInstr::FORMAT_I, FuncInstr::COND_BRANCH },

    //Unconditional jump

    { "j",     0x2,     0x00,  FuncInstr::FORMAT_J, FuncInstr::UNCOND_BRANCH },
    { "jr",    0x0,     0x08,  FuncInstr::FORMAT_R, FuncInstr::UNCOND_BRANCH },

};

//MIPS register file

const std::string FuncInstr::RegFile[NUM_OF_REGISTERS] =
{
    "$zero",                                                     //constant 0
	"$at", 	                                                     //assembler temporary
    "$v0", "$v1", 	                                             //not used
    "$a0", "$a2", "$a3", 	                                     //not used
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 	 //temporaries
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 	  //not used
    "$t8", "$t9", 	                                             //temporaries
    "$k0", "$k1", 	                                             //not used
    "$gp", 	                                                     //not used
    "$sp", 	                                                     //not used
    "$fp", 	                                                     //not used
   	"$ra" 	                                                     //not used
};


//-----------------------------------------------------------------------------------------
//Internal methods for parsing
//-----------------------------------------------------------------------------------------

void FuncInstr::initFormat( uint32 bytes)
{
    this->bytes.raw = bytes;
    uint32 opcode = this->bytes.asR.opcode;            //get opcode of instruction

    bool find_flag = false;

    for (int num_of_instr = 0; num_of_instr < this->NUM_OF_COMMANDS; num_of_instr++)
    {
        if (this->isaTable[num_of_instr].opcode == opcode)
        {
            this->format = this->isaTable[num_of_instr].format;
            find_flag = true;
            break;
        }
    }

    MY_ASSERT(find_flag, "Unknown format of instruction");
}

//inicialization of R-format instruction

void FuncInstr::initR( uint32 bytes)
{
    std::string indent;          // temp string
    std::ostringstream oss;      //temp stream

    // handling of pseudo instructions

    if ((this->bytes.asR.funct == 0x21) && (this->bytes.asR.rt == 0) && (this->bytes.asR.rs == 0))    // clear instruction
    {
        oss << indent << "clear" << " " << this->RegFile[this->bytes.asR.rd] << std::endl;
        this->OUT = oss.str();
        return;
    }

    if (this->bytes.raw == 0)                 // nop
    {
        oss << indent << "nop" << std::endl;
        this->OUT = oss.str();
        return;
    }

    // handling of usual instruction

    this->parseR(bytes);
}

//inicialization of I-format instruction

void FuncInstr::initI( uint32 bytes)
{
    std::string indent;          // temp string
    std::ostringstream oss;      //temp stream

    // handling of pseudo instructions

    if ((this->bytes.asI.opcode == 0x9) && (this->bytes.asI.imm == 0))    // clear instruction
    {
        oss << indent << "move" << " " << this->RegFile[this->bytes.asI.rt] << " " << this->RegFile[this->bytes.asI.rs] << " " << std::endl;
        this->OUT = oss.str();
        return;
    }

    // handling of usual instruction

    this->parseI(bytes);
}

//inicialization of J-format instruction

void FuncInstr::initJ( uint32 bytes)
{
    std::string indent;          // temp string
    std::ostringstream oss;      //temp stream

    // handling of pseudo instructions
    // NO PSEUDO INSTRUCTION OF J_FORMAT

    // handling of usual instruction

    this->parseJ(bytes);
}

// Parse R-format instruction

void FuncInstr::parseR( uint32 bytes)
{

    std::string indent;          // temp string
    std::ostringstream oss;      //temp stream


    uint32 opcode = this->bytes.asR.opcode;
    uint32 rt = this->bytes.asR.rt;
    uint32 rs = this->bytes.asR.rs;
    uint32 rd = this->bytes.asR.rd;
    uint32 shamt = this->bytes.asR.shamt;
    uint32 funct = this->bytes.asR.funct;

    //find name of instructioin in isaTable

    bool find_flag = false;

    int num_of_instr = 0;
    for(num_of_instr = 0; num_of_instr < this->NUM_OF_COMMANDS; num_of_instr++)
    {
        if ((this->isaTable[num_of_instr].opcode == opcode) && (this->isaTable[num_of_instr].funct == funct))
        {
            oss << indent << this->isaTable[num_of_instr].name;
            find_flag = true;
            break;
        }
    }

    MY_ASSERT(find_flag, "Unknown format of instruction");

   //----------------------------------------------------------------------------------------------------------

    switch (this->isaTable[num_of_instr].type)
    {
        case ADD_OR_SUB:
            oss << indent << " " << this->RegFile[rd] << " " << this->RegFile[rt] << " " << this->RegFile[rs];
            oss << indent << std::endl;
            this->OUT = oss.str();
            break;

        case SHIFT:
            oss << indent << " " << this->RegFile[rd] << " " << this->RegFile[rt] << " " << shamt;
            oss << indent << std::endl;
            this->OUT = oss.str();
            break;

        case UNCOND_BRANCH:
            oss << indent << " " << this->RegFile[rs];
            oss << indent << std::endl;
            this->OUT = oss.str();
            break;
        default:
            MY_ASSERT(NULL, "Unknown format of instruction");
    }

}

//Parse I-format instruction

void FuncInstr::parseI( uint32 bytes)
{

    std::string indent;          // temp string
    std::ostringstream oss;      //temp stream

    uint32 opcode = this->bytes.asI.opcode;
    uint32 rt = this->bytes.asI.rt;
    uint32 rs = this->bytes.asI.rs;
    uint32 imm = this->bytes.asI.imm;

    //find name of instructioin in isaTable

    bool find_flag = false;

    int num_of_instr = 0;
    for(num_of_instr = 0; num_of_instr < this->NUM_OF_COMMANDS; num_of_instr++)
    {
        if (this->isaTable[num_of_instr].opcode == opcode)
        {
            oss << indent << this->isaTable[num_of_instr].name;
            find_flag = true;
            break;
        }
    }

    MY_ASSERT(find_flag, "Unknown format of instruction");

    //------------------------------------------------------------------------------------------------------

    switch (this->isaTable[num_of_instr].type)
    {
        case ADD_OR_SUB:
            oss << indent << " " << this->RegFile[rt] << " " << this->RegFile[rs] << " " << imm;
            oss << indent << std::endl;
            this->OUT = oss.str();
            break;

        case COND_BRANCH:
            oss << indent << " " << this->RegFile[rs] << " " << this->RegFile[rt] << " " << imm;
            oss << indent << std::endl;
            this->OUT = oss.str();
            break;
        default:
            MY_ASSERT(NULL, "Unknown format of instruction");
    }
}

//Parse J-format instruction

void FuncInstr::parseJ( uint32 bytes)
{

    std::string indent;          // temp string
    std::ostringstream oss;      //temp stream

    uint32 opcode = this->bytes.asJ.opcode;
    uint32 offset = this->bytes.asJ.offset;

    //find name of instructioin in isaTable

    bool find_flag = false;

    int num_of_instr = 0;
    for(num_of_instr = 0; num_of_instr < this->NUM_OF_COMMANDS; num_of_instr++)
    {
        if (this->isaTable[num_of_instr].opcode == opcode)
        {
            oss << indent << this->isaTable[num_of_instr].name;
            find_flag = true;
            break;
        }
    }

    MY_ASSERT(find_flag, "Unknown format of instruction");

    //----------------------------------------------------------------------------------------------------------

    oss << indent << offset;
    oss << indent << std::endl;
    this->OUT = oss.str();

}

//Dump function

std::string FuncInstr::Dump() const
{
    return this->OUT;
}
