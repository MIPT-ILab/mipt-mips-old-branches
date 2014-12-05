#include <func_instr.h>
#include <sstream>
#include <iomanip>
#include <cstring>


FuncInstr::FuncInstr( uint32 bytes):
    opcMask( 0xff << 26),
    funcMask( 0x3f)
{
    this->InitFormat( bytes);
    switch ( format)
    {
        case FORMAT_R:
            ParseR();
            break;
        case FORMAT_I:
            ParseI();
            break;
        case FORMAT_J:
            ParseJ();
            break;
        default:
            assert( 0);
    }
}

FuncInstr:: ~FuncInstr()
{
    delete[] name;
    delete[] reg1;
    delete[] reg2;
    delete[] reg3;
}


void FuncInstr::InitFormat( uint32 bytes)
{
    this->bytes.raw = bytes;
    switch ( this->bytes.asR.opcode)
    {
        case 0x0:
            format = FORMAT_R;
            break;
        case 0x2:
            format = FORMAT_J;
            break;
        default:
            format = FORMAT_I;
            break;
    }
}

void FuncInstr::ParseR()
{
    strcpy( reg1, regTable[bytes.asR.d].name);
    strcpy( reg3, regTable[bytes.asR.t].name);
    switch ( bytes.asR.funct)
    {
        case 0x20:
            strcpy( name, isaTable[ADD].name);
            strcpy( reg2, regTable[bytes.asR.s1].name);
            break;
        case 0x21:
            strcpy( name, isaTable[ADDU].name);
            strcpy( reg2, regTable[bytes.asR.s1].name);
            break;
        case 0x22:
            strcpy( name, isaTable[SUB].name);
            strcpy( reg2, regTable[bytes.asR.s1].name);
            break;
        case 0x23:
            strcpy( name, isaTable[SUBU].name);
            strcpy( reg2, regTable[bytes.asR.s1].name);
            break;
        case 0x0:
            strcpy( name, isaTable[SLL].name);
            strcpy( reg2, regTable[bytes.asR.s2].name);
            break;
        case 0x2:
            strcpy( name, isaTable[SRL].name);
            strcpy( reg2, regTable[bytes.asR.s2].name);
            break;
        default: 
            assert( 0);
    }
}

void FuncInstr::ParseI()
{
    strcpy( reg1, regTable[bytes.asI.t].name);
    strcpy( reg2, regTable[bytes.asI.s].name);
    cnst = bytes.asI.imm;
    switch ( bytes.asI.opcode )
    {
        case 0x8:
            strcpy( name, isaTable[ADDI].name);
            break;
        case 0x9:
            strcpy( name, isaTable[ADDIU].name);
            break;
        case 0x4:
            strcpy( name, isaTable[BEQ].name);
            break;
        default:
            assert( 0);
    }
}

void FuncInstr::ParseJ()
{
    switch ( bytes.asJ.opcode )
    {
        case 0x2:
            strcpy( name, isaTable[J].name);
            cnst = bytes.asJ.addr;
            break;
        default:
            assert( 0);
    }
}

std::string FuncInstr::Dump( std::string indent = " ") const
{
    std::ostringstream oss;
    oss << indent << endl << name;
    switch ( format )
    {
        case FORMAT_R:
            oss << reg1 << "," << reg2 << "," << reg3 << endl;
            break;
        case FORMAT_I:
            oss << reg1 << "," << reg2 << "," << cnst << endl;
            break;
        case FORMAT_J:
            oss << cnst << endl;
            break;
        default:
            assert( 0);
    }
    return oss.str();
}

std::ostream& operator<<( std::ostream& out, FuncInstr& instr)
{
    return out << instr.Dump("");
}