 /**
 * created by Alexander Pronin <alexander.pronin.mipt@gmail.com>
 *
 * v. 1.0: created 05.12.2014 22:06
 *         first version of MIPS disasm
 *
 */

// Generic C
#include <string.h>

// Generic C++
#include <sstream>
#include <iomanip>

// uArchSim modules
#include <func_memory.h>
#include <func_instr.h>

const ISAEntry isaTable[ lastType + 1] =
{
    // name     opcode    func   format    type
    { "add", 0x0, 0x20, FORMAT_R, ADD},
    { "addu", 0x0, 0x21, FORMAT_R, ADDU},
    { "sub", 0x0, 0x22, FORMAT_R, SUB},
    { "subu", 0x0, 0x23, FORMAT_R, SUBU},
    { "addi", 0x8, 0x0, FORMAT_I, ADDI},
    { "addiu", 0x9, 0x0, FORMAT_I, ADDIU},

    { "sll", 0x0, 0x0, FORMAT_R, SLL},
    { "srl", 0x0, 0x2, FORMAT_R, SRL},

    { "beq", 0x4, 0x0, FORMAT_I, BEQ},
    { "bne", 0x5, 0x0, FORMAT_I, BNE},

    { "j", 0x2, 0x0, FORMAT_J, J},
    { "jr", 0x0, 0x8, FORMAT_R, JR},

    { "lb", 0x20, 0x0, FORMAT_I, LB},
    { "lw", 0x23, 0x0, FORMAT_I, LW},

    { "sb", 0x28, 0x0, FORMAT_I, SB},
    { "sw", 0x2b, 0x0, FORMAT_I, SW}
};

const RegEntry regTable[ 32] =
{
    // name    address
    { "$zero",  0x0 },
    { "$at",    0x1 },
    { "$v0",    0x2 },
    { "$v1",    0x3 },
    { "$a0",    0x4 },
    { "$a1",    0x5 },
    { "$a2",    0x6 },
    { "$a3",    0x7 },
    { "$t0",    0x8 },
    { "$t1",    0x9 },
    { "$t2",    0xA },
    { "$t3",    0xB },
    { "$t4",    0xC },
    { "$t5",    0xD },
    { "$t6",    0xE },
    { "$t7",    0xF },
    { "$s0",    0x10},
    { "$s1",    0x11},
    { "$s2",    0x12},
    { "$s3",    0x13},
    { "$s4",    0x14},
    { "$s5",    0x15},
    { "$s6",    0x16},
    { "$s7",    0x17},
    { "$t8",    0x18},
    { "$t9",    0x19},
    { "$k0",    0x1A},
    { "$k1",    0x1B},
    { "$gp",    0x1C},
    { "$sp",    0x1D},
    { "$fp",    0x1E},
    { "$ra",    0x1F}
};

FuncInstr::FuncInstr( uint32 bytes)
{
    this->operation.raw = bytes;
    this->initFormat( bytes);
}

int FuncInstr::initFormat( uint32 bytes)
{
    for ( int i = 0; i < lastType; i++)
    {
        if ( this->operation.asR.opcode == isaTable[ i].opcode)
        {
            this->format = isaTable[ i].format;
            if ( this->format != FORMAT_R)
            {
                this->type = isaTable[ i].type;
                return 0;
            }
            if ( this->operation.asR.func == isaTable[ i].func)
            {
                this->type = isaTable[ i].type;
                return 0;
            }
        }
    }
    std::cerr << "ERROR.*";
    exit( EXIT_FAILURE);
}

std::string FuncInstr::Dump( std::string indent) const
{
    std::ostringstream oss;
    oss << indent << isaTable[ this->type].name;
    switch ( this->type)
    {
    case ADD:
        oss << " "  << regTable[ this->operation.asR.d].name
            << ", " << regTable[ this->operation.asR.s].name
            << ", " << regTable[ this->operation.asR.t].name;
        break;
    case ADDU:
        oss << " "  << regTable[ this->operation.asR.d].name
            << ", " << regTable[ this->operation.asR.s].name
            << ", " << regTable[ this->operation.asR.t].name;
        break;
    case SUB:
        oss << " "  << regTable[ this->operation.asR.d].name
            << ", " << regTable[ this->operation.asR.s].name
            << ", " << regTable[ this->operation.asR.t].name;
        break;
    case SUBU:
        oss << " "  << regTable[ this->operation.asR.d].name
            << ", " << regTable[ this->operation.asR.s].name
            << ", " << regTable[ this->operation.asR.t].name;
        break;
    case ADDI:
        oss << " "  << regTable[ this->operation.asI.t].name
            << ", " << regTable[ this->operation.asI.s].name
            << ", " << hexi( this->operation.asI.imm);
        break;
    case ADDIU:
        oss << " "  << regTable[ this->operation.asI.t].name
            << ", " << regTable[ this->operation.asI.s].name
            << ", " << hexi( this->operation.asI.imm);
        break;
    case SLL:
        oss << " "  << regTable[ this->operation.asR.d].name
            << ", " << regTable[ this->operation.asR.t].name
            << ", " << hexi( this->operation.asR.shamt);
        break;
    case SRL:
        oss << " "  << regTable[ this->operation.asR.d].name
            << ", " << regTable[ this->operation.asR.t].name;
        break;
    case BEQ:
        oss << " "  << regTable[ this->operation.asI.s].name
            << ", " << regTable[ this->operation.asI.t].name
            << ", " << hexi( this->operation.asI.imm);
        break;
    case BNE:
        oss << " "  << regTable[ this->operation.asI.s].name
            << ", " << regTable[ this->operation.asI.t].name
            << ", " << hexi( this->operation.asI.imm);
        break;
    case J:
        oss << " " << hexi( this->operation.asJ.addr);
        break;
    case JR:
        oss << " " << regTable[ this->operation.asR.s].name;
        break;
    case LB:
        oss << " "  << regTable[ this->operation.asI.t].name
            << ", " << hexi( this->operation.asI.imm)
            << "("  << regTable[ this->operation.asI.s].name
            << ")";
        break;
    case LW:
        oss << " "  << regTable[ this->operation.asI.t].name
            << ", " << hexi( this->operation.asI.imm)
            << "("  << regTable[ this->operation.asI.s].name
            << ")";
        break;
    case SB:
        oss << " "  << regTable[ this->operation.asI.t].name
            << ", " << hexi( this->operation.asI.imm)
            << "("  << regTable[ this->operation.asI.s].name
            << ")";
        break;
    case SW:
        oss << " "  << regTable[ this->operation.asI.t].name
            << ", " << hexi( this->operation.asI.imm)
            << "("  << regTable[ this->operation.asI.s].name
            << ")";
        break;
    }
    return oss.str();
}

std::string hexi( unsigned val)
{
    std::ostringstream oss;
    oss << "0x" << hex << val;
    return oss.str();
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump(""); 
    return out;
}
