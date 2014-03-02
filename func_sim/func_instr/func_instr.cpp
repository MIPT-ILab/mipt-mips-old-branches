/**
 * implementation of class FuncInstr
 *
 *
 */


#include "func_instr.h"


FuncInstr::FuncInstr( uint32 bytes)
{
    this->initFormat( bytes);
    switch ( this->format)
    {
        case FORMAT_R:
            this->parseR( );
            break;
        case FORMAT_J:
            this->parseJ( );
            break;
        case FORMAT_I:
            this->parseI( );
            break;
        default:
            assert(0);
    }
}

int FuncInstr::initFormat( uint32 bytes)
{
    this->bytes.raw = bytes;
    uint8 opcode = this->bytes.asR.opcode;

    for( int i = 0;
      this->isaTable[i].type != END_OF_INSTRUCTIONS_SET;
      i++)
    {
        // if we can identify only on opcode!
        if ( opcode == this->isaTable[i].opcode)
        {
            this->format = this->isaTable[i].format;
            this->type   = this->isaTable[i].type;
            return 0;
        }
    }
    return -1;
}

int FuncInstr::parseR()
{
    uint8 opcode = this->bytes.asR.opcode;
    uint8 funct = this->bytes.asR.funct;
    uint32 bytes = this->bytes.raw;
    int i = 0;
    while(
     !( opcode== this->isaTable[i].opcode && funct== this->isaTable[i].func)
     || 
     (this->isaTable[i].type!= END_OF_INSTRUCTIONS_SET) )
        i++;

    if ( bytes == 0)                     //------- nop
    {
        sprintf(textAss,"nop\n");
        return 0;
    }
    else if( !opcode && (funct==0x21) &&  //------- clear
             !this->bytes.asR.s && !this->bytes.asR.t )
    {
        sprintf(textAss,"clear %s\n",
            this->registersTable[this->bytes.asR.d]);
        return 0;
    }
    this->type = this->isaTable[i].type;
    //-------------- name %1 %2 %3
    if( this->type == ADD 
        ||
        this->type == SUB)
    {
        sprintf(textAss,"%s %s %s %s\n",
                this->isaTable[i].name,
                this->registersTable[this->bytes.asR.d],
                this->registersTable[this->bytes.asR.s],
                this->registersTable[this->bytes.asR.t]);
        return 0;
    }
    //-------------- name %1 %2 C
    if( this->type ==SHIFT)
    {
        sprintf(textAss,"%s %s %s %u\n",
                this->isaTable[i].name,
                this->registersTable[this->bytes.asR.d],
                this->registersTable[this->bytes.asR.t],
                (uint32) this->bytes.asR.shamt);
        return 0;
    }
    return -1;
}

int FuncInstr::parseI()
{
    uint8 opcode = this->bytes.asR.opcode;
    uint32 bytes = this->bytes.raw;
    int i = 0;
    while(
     !( opcode== this->isaTable[i].opcode)
     || 
     (this->isaTable[i].type!= END_OF_INSTRUCTIONS_SET) ) 
        i++;
    if ( (opcode == 0x9) && !this->bytes.asI.imm)//----move
    {
        sprintf(textAss,"move %s %s\n",
                this->registersTable[this->bytes.asI.s],
                this->registersTable[this->bytes.asI.t]);
        return 0;
    }
    if ( this->type == BRANCH
         || 
         this->type == ADD )
    {
        sprintf(textAss,"%s %s %s %u\n",
                this->isaTable[i].name,
                this->registersTable[this->bytes.asI.s],
                this->registersTable[this->bytes.asI.t],
                (uint32) this->bytes.asI.imm);///??????????????????????????
        return 0;
    }
    return -1;
}


int FuncInstr::parseJ()
{
    uint8 opcode = this->bytes.asR.opcode;
    uint32 bytes = this->bytes.raw;
    int i = 0;
    while(
     !( opcode== this->isaTable[i].opcode)
     || 
     (this->isaTable[i].type!= END_OF_INSTRUCTIONS_SET) )
        i++;
    
    if ( this->type == BRANCH)
    {
        sprintf(textAss,"%s %u\n",
                this->isaTable[i].name,
                (uint32) this->bytes.asJ.address);
        return 0;
    }
    return -1;
}

const FuncInstr::ISAEntry FuncInstr::isaTable[] = 
{
    // name   opcode    func  format               type
    { "add  ", 0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD    },
    { "addu" , 0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD    },
    { "sub"  , 0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB    },
    { "subu" , 0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUB    },
    { "addi" , 0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD    },
    { "addiu", 0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD    },
    { "sll"  , 0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::SHIFT  },
    { "srl"  , 0x0,     0x02, FuncInstr::FORMAT_R, FuncInstr::SHIFT  },
    { "beq"  , 0x4,     0x00, FuncInstr::FORMAT_I, FuncInstr::BRANCH },
    { "bne"  , 0x5,     0x00, FuncInstr::FORMAT_I, FuncInstr::BRANCH },
    { "j"    , 0x2,     0x00, FuncInstr::FORMAT_J, FuncInstr::BRANCH },
    { "jr"   , 0x0,     0x08, FuncInstr::FORMAT_R, FuncInstr::BRANCH },
    // the last one must be of "END_OF..." type
    { "",0x0,0x0,FuncInstr::FORMAT_R ,FuncInstr::END_OF_INSTRUCTIONS_SET}

};

const char* FuncInstr::registersTable[] =
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




