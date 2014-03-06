/**
 * implementation of class FuncInstr
 *
 *
 */

#include "func_instr.h"
#include <sstream>

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
            std::ostringstream oss;
            oss << "Undefined command\n";
            this->textAss = oss.str();
    }
}

// This function thinks about what temtlate use
// to disasembly this opcode
int FuncInstr::initFormat( uint32 bytes)
{
    this->bytes.raw = bytes;
    uint8 opcode = this->bytes.asR.opcode;
    for( int i = 0; this->isaTable[i].type != END_OF_INSTRUCTIONS_SET; i++)
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
    std::ostringstream oss;
    int i = 0;
    while(
     !( opcode== this->isaTable[ i].opcode && funct== this->isaTable[ i].func)
     && 
     ( this->isaTable[ i].type != END_OF_INSTRUCTIONS_SET) )
        i++;

     if ( this->isaTable[i].type== END_OF_INSTRUCTIONS_SET)
        {
            oss << "Undefined command\n";
            this->textAss = oss.str();
            return -1;
        }
    if ( bytes == 0)                     //------- nop
    {
        oss << "nop\n";
        this->textAss = oss.str();
        return 0;
    }
    if( !opcode && (funct==0x21) &&     //------- clear
             !this->bytes.asR.s && !this->bytes.asR.t )
    {
        oss << "clear ";
        oss  << this->registersTable[this->bytes.asR.d];
        this->textAss = oss.str();
        return 0;
    }
    
    this->type = this->isaTable[i].type;
    switch (this->type) 
    {
        //-------------- name %1 %2 %3
        case ADD:
        case SUB:
            oss << this->isaTable[i].name;
            oss << this->registersTable[this->bytes.asR.d];
            oss << this->registersTable[this->bytes.asR.s];
            oss << this->registersTable[this->bytes.asR.t] <<"\n";
            break;
        //-------------- name %1 %2 C
        case SHIFT:
            oss << this->isaTable[i].name;
            oss << this->registersTable[this->bytes.asR.d];
            oss << this->registersTable[this->bytes.asR.t];
            oss << (uint32) this->bytes.asR.shamt <<"\n";
            break;
        //-------------- jr $s
        case BRANCH_R:
            oss << this->isaTable[i].name;
            oss <<this->registersTable[this->bytes.asR.s] <<"\n";
            break;
        default:
            oss <<"Undefined command\n";
            this->textAss = oss.str();
            return -1;
    }
    this->textAss = oss.str();
    return 0;
}

int FuncInstr::parseI()
{
    uint8 opcode = this->bytes.asI.opcode;
    uint32 bytes = this->bytes.raw;
    std::ostringstream oss;
    int i = 0;
    while(
     !( opcode== this->isaTable[i].opcode)
     && 
     ( this->isaTable[ i].type!= END_OF_INSTRUCTIONS_SET) ) 
        i++;
    
     if ( this->isaTable[ i].type== END_OF_INSTRUCTIONS_SET)
        {
            oss <<"Undefined command\n";
            this->textAss = oss.str();
            return -1;
        }
    if ( (opcode == 0x9) && ( this->bytes.asI.imm == 0))//----move
    {
        oss << "move " << this->registersTable[ this->bytes.asI.t];
        oss << this->registersTable[ this->bytes.asI.s] <<"\n";
        this->textAss = oss.str();
        return 0;
    }
    
    this->type = this->isaTable[ i].type;
    switch ( this->type)
    {
        case BRANCH:
            oss << this->isaTable[i].name;
            oss << this->registersTable[ this->bytes.asI.s];
            oss << this->registersTable[ this->bytes.asI.t];
            oss << (int16) this->bytes.asI.imm << "\n";
            break;
        case ADD:
            oss << this->isaTable[ i].name;
            oss << this->registersTable[ this->bytes.asI.t];
            oss << this->registersTable[ this->bytes.asI.s];
            oss << (int16) this->bytes.asI.imm << "\n";
            break;
        case ADDU:
            oss << this->isaTable[ i].name;
            oss << this->registersTable[ this->bytes.asI.t];
            oss << this->registersTable[ this->bytes.asI.s];
            oss << (uint16) this->bytes.asI.imm <<"\n";
            break;
        default:
            oss << "Undefined command\n";
            this->textAss = oss.str();
            return -1;
    }
    this->textAss = oss.str();
    return 0;
}

int FuncInstr::parseJ()
{
    uint8 opcode = this->bytes.asR.opcode;
    uint32 bytes = this->bytes.raw;
    std::ostringstream oss;
    int i = 0;
    while(
     !( opcode== this->isaTable[i].opcode)
     && 
     (this->isaTable[i].type!= END_OF_INSTRUCTIONS_SET) )
        i++;
    if (this->isaTable[i].type == END_OF_INSTRUCTIONS_SET)
    {
        oss <<"Undefined command\n";
        this->textAss = oss.str();
        return -1;
    }
    
    this->type = this->isaTable[ i].type;
    switch (this->type)
    {
        case BRANCH:
            oss << this->isaTable[ i].name <<"\n";
            oss << (uint32) this->bytes.asJ.address;
            break;
        default:
            oss << "Undefined command\n";
            this->textAss = oss.str();
            return -1;
    }
    this->textAss = oss.str();
    return 0;
}

std::string FuncInstr::Dump( std::string indent) const
{
    return indent+this->textAss;
}

std::ostream& operator << ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump("");
}

const FuncInstr::ISAEntry FuncInstr::isaTable[] = 
{
    // name   opcode    func  format               type
    { "add "  , 0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD    },
    { "addu " , 0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD    },
    { "sub "  , 0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB    },
    { "subu " , 0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUB    },
    { "addi " , 0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD    },
    { "addiu ", 0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADDU   },
    { "sll "  , 0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::SHIFT  },
    { "srl "  , 0x0,     0x02, FuncInstr::FORMAT_R, FuncInstr::SHIFT  },
    { "beq "  , 0x4,     0x00, FuncInstr::FORMAT_I, FuncInstr::BRANCH },
    { "bne "  , 0x5,     0x00, FuncInstr::FORMAT_I, FuncInstr::BRANCH },
    { "j "    , 0x2,     0x00, FuncInstr::FORMAT_J, FuncInstr::BRANCH },
    { "jr "   , 0x0,     0x08, FuncInstr::FORMAT_R, FuncInstr::BRANCH_R },
    // the last one must be of "END_OF..." type
    { "",0xf,0x0,FuncInstr::FORMAT_R ,FuncInstr::END_OF_INSTRUCTIONS_SET}

};

const char* FuncInstr::registersTable[] =
{
    "$zero ",
    "$at ",
    "$v0 ", "$v1 ",
    "$a0 ", "$a1 ", "$a2 ", "$a3 ",
    "$t0 ", "$t1 ", "$t2 ", "$t3 ", "$t4 ", "$t5 ", "$t6 ", "$t7 ",
    "$s0 ", "$s1 ", "$s2 ", "$s3 ", "$s4 ", "$s5 ", "$s6 ", "$s7 ",
    "$t8 ", "$t9 ",
    "$k0 ", "$k1 ",        
    "$gp ",
    "$sp ",
    "$fp ",
    "$ra "
};


