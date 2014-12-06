#include <func_instr.h>


FuncInstr::FuncInstr( uint32 bytes)
{
    this->bytes.raw;
    this->initFormat( bytes);
    switch ( this->format)
    {
        case FORMAT_R:
            for ( int i = 0; i < numCom; i++)
                if (( this->bytes.asR.opcode == isaTable[ i].opcode) && ( this->bytes.asR.funct == isaTable[ i].funct)) 
                { 
                    this->type = isaTable [ i].type;
                    break;
                }
            break;
        case FORMAT_J:
            for ( int i = 0; i < numCom; i++)
                if ( this->bytes.asJ.opcode == isaTable[ i].opcode)
                {    
                    this->type = isaTable [ i].type;
                    break;
                }
            break;
        case FORMAT_I:
            for ( int i = 0; i < numCom; i++)
                if ( this->bytes.asI.opcode == isaTable[ i].opcode)
                {
                    this->type = isaTable [ i].type;
                    break;
                }

            break;
        default:
            assert (0);
    }
}

void FuncInstr::parseI( uint32 bytes)
{

}

void FuncInstr::parseR( uint32 bytes)
{

}

void FuncInstr::parseJ( uint32 bytes)
{

}

void FuncInstr::initFormat ( uint32 bytes)
{
    switch ( this->bytes.asR.opcode)
    {
        case 0x0:
            this->format = FORMAT_R;
            break;
        case 0x2:
            this->format = FORMAT_J;
            break;
        case 0x4:
        case 0x5:
        case 0x8:
        case 0x9:
            this->format = FORMAT_I;
            break;
        default:
            assert(0);
    }  

}

const FuncInstr::ISAEntry isaTable[ numCom]=
{
    // name   opcode    func      format              type
    { "add  ",  0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD },
    { "addu ",  0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU },
    { "sub ",   0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB },
    { "subu ",  0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU },
    { "addi ",  0x8,      0x0, FuncInstr::FORMAT_I, FuncInstr::ADDI },
    { "addiu ", 0x9,      0x0, FuncInstr::FORMAT_I, FuncInstr::ADDIU },
    
    { "sll ",   0x0,      0x0, FuncInstr::FORMAT_R, FuncInstr::SLL },
    { "srl ",   0x0,      0x2, FuncInstr::FORMAT_R, FuncInstr::SRL },
    
    { "beq ",   0x4,      0x0, FuncInstr::FORMAT_I, FuncInstr::BEQ },
    { "bne ",   0x5,      0x0, FuncInstr::FORMAT_I, FuncInstr::BNE },
    
    { "j ",     0x2,      0x0, FuncInstr::FORMAT_J, FuncInstr::JUMP },
    { "jr ",    0x0,      0x8, FuncInstr::FORMAT_R, FuncInstr::JUMPR }
};

const FuncInstr::REGEntry regTable [ 32] = 
{
    { "$zero",  0x0},
    {   "$at",  0x1},
    {   "$v0",  0x2},
    {   "$v1",  0x3},
    {   "$a0",  0x4},
    {   "$a1",  0x5},
    {   "$a2",  0x6},
    {   "$a3",  0x7},
    {   "$t0",  0x8},
    {   "$t1",  0x9},
    {   "$t2",  0xA},
    {   "$t3",  0xB},
    {   "$t4",  0xC},
    {   "$t5",  0xD},
    {   "$t6",  0xE},
    {   "$t7",  0xF},
    {   "$s0", 0x10},
    {   "$s1", 0x11},
    {   "$s2", 0x12},
    {   "$s3", 0x13},
    {   "$s4", 0x14},
    {   "$s5", 0x15},
    {   "$s6", 0x16},
    {   "$s7", 0x17},
    {   "$t8", 0x18},
    {   "$t9", 0x19},
    {   "$k0", 0x1A},
    {   "$k1", 0x1B},
    {   "$gp", 0x1C},
    {   "$sp", 0x1D},
    {   "$fp", 0x1E},
    {   "$ra", 0x1F}
};

std::ostream& operator<<(std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump("");
    return out;
}

std::string FuncInstr::Dump ( std::string intend) const
{
    std::ostringstream oss;
    oss << intend << isaTable[ this->type].name << " ";
    switch ( this->type)
    {
        case ADD:
        case ADDU:
        case SUB:
        case SUBU:
            oss << regTable [ this->bytes.asR.s].name << ", " << regTable [ this->bytes.asR.t].name << ", " << regTable [ this->bytes.asR.d].name << "\n";
            break;
        case ADDI:
        case ADDIU:
            oss << regTable [ this->bytes.asI.s].name << ", " << regTable [ this->bytes.asI.t].name << ", " << hex << this->bytes.asI.imm << dec << "\n";
            break;
        case SLL:
        case SRL:
            oss << regTable [ this->bytes.asR.s].name << ", " << regTable [ this->bytes.asR.t].name << ", " << hex << this->bytes.asR.shamt << dec << "\n";
            break;
        case JUMP:
            oss << hex << this->bytes.asI.imm << dec << "\n";
        case JUMPR:
            oss << regTable [ this->bytes.asR.s].name << "\n";
            break;
        default:
            assert(0);
    }
}
