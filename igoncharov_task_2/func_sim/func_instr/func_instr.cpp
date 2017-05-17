// Generic C
#include <string.h>

// Generic C++
#include <iomanip>
#include <sstream>

// uArchSim modules
#include <func_instr.h>
#include <func_memory.h>

FuncInstr::FuncInstr( uint32 bytes)
{
    this->operation.raw = bytes;

    for ( int i = 0; i < LAST; ++i)
    {
        if ( this->bytes.asR.opcode == isaTable[ i].opcode)
        {
            this->format = isaTable[ i].format;

            switch (this->format)
            case FORMAT_R:
                if ( this->bytes.asR.funct == isaTable[ i].funct)
                    this->type = isaTable[ i].type;
                return 0;
            break;
            case FORMAT_I:
            case FORMAT_J:
                this->type = isaTable[ i].type;
                return 0;
            break;
        }
    }
    assert(0);
}

std::string FuncInstr::Dump( std::string indent) const
{
    std::ostringstream oss;

    oss << indent << isaTable[ this->type].name;

    if ( this->format == FORMAT_R)
    {
            switch ( this->type)
            {
                case ADD:
                oss << " "  << this->regTable[ this->bytes.asR.d].name
                    << ", " << this->regTable[ this->bytes.asR.s].name
                    << ", " << this->regTable[ this->bytes.asR.t].name;
                break;
                case ADDU:
                oss << " "  << this->regTable[ this->bytes.asR.d].name
                    << ", " << this->regTable[ this->operation.asR.s].name
                    << ", " << this->regTable[ this->operation.asR.t].name;
                break;
                case SUB:
                oss << " "  << this->regTable[ this->operation.asR.d].name
                    << ", " << this->regTable[ this->operation.asR.s].name
                    << ", " << this->regTable[ this->operation.asR.t].name;
                break;
                case SUBU:
                oss << " "  << this->regTable[ this->operation.asR.d].name
                    << ", " << this->regTable[ this->operation.asR.s].name
                    << ", " << this->regTable[ this->operation.asR.t].name;
                break;
                case SLL:
                oss << " "  << this->regTable[ this->operation.asR.d].name
                    << ", " << this->regTable[ this->operation.asR.t].name
                    << ", " << "0x" << hex << this->operation.asR.shamt;
                break;
                case SRL:
                oss << " "  << this->regTable[ this->operation.asR.d].name
                    << ", " << this->regTable[ this->operation.asR.t].name;
                    << ", " << "0x" << hex << this->operation.asR.shamt;
                break;
                case JR:
                oss << " " << this->regTable[ this->operation.asR.s].name;
                break;
            }
    }
    else if (this->format == FORMAT_I)
    {
            switch ( this->type)
            {
                case ADDI:
                oss << " "  << this->regTable[ this->operation.asI.t].name
                    << ", " << this->regTable[ this->operation.asI.s].name
                    << ", " << "0x" << hex << this->operation.asI.imm;
                break;
                case ADDIU:
                oss << " "  << this->regTable[ this->operation.asI.t].name
                    << ", " << this->regTable[ this->operation.asI.s].name
                    << ", " << "0x" << hex << this->operation.asI.imm;
                break;
                case BEQ:
                oss << " "  << this->regTable[ this->operation.asI.s].name
                    << ", " << this->regTable[ this->operation.asI.t].name
                    << ", " << "0x" << hex <<  this->operation.asI.imm;
                break;
                case BNE:
                oss << " "  << this->regTable[ this->operation.asI.s].name
                    << ", " << this->regTable[ this->operation.asI.t].name
                    << ", " << "0x" << hex << this->operation.asI.imm;
                break;
            }
    }
    else if (this->format == FORMAT_J)
    {
            switch ( this->type)
            {
                case J:
                oss << " " << "0x" << hex << this->operation.asJ.addres;
                break;
            }
    }


    return oss.str();
}



std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump("");
    return out;
}
