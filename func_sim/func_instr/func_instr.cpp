#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::string;
using std::endl;

#include <assert.h>

#include <func_instr.h>


struct ISAEntry
{
    const char* name;

    uint8 opcode;
    uint8 func;

    FuncInstr::Format format;
    FuncInstr::Type type;
};
#define ISA_TABLE_SIZE 12
static const ISAEntry isaTable[] = {
        // name   opcode    func      format              type
        { "add "  , 0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD   },
        { "addu " , 0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD   },
        { "sub "  , 0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB   },
        { "subu " , 0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUB   },
        { "addi " , 0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD   },
        { "addiu ", 0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD   },
        { "sll "  , 0x0,     0x0 , FuncInstr::FORMAT_R, FuncInstr::SHIFT },
        { "srl "  , 0x0,     0x2 , FuncInstr::FORMAT_R, FuncInstr::SHIFT },
        { "beq "  , 0x4,     0x0 , FuncInstr::FORMAT_I, FuncInstr::BRANCH},
        { "bne "  , 0x5,     0x0 , FuncInstr::FORMAT_I, FuncInstr::BRANCH},
        { "j "    , 0x2,     0x0 , FuncInstr::FORMAT_J, FuncInstr::JUMP  },
        { "jr "   , 0x0,     0x8 , FuncInstr::FORMAT_R, FuncInstr::JUMP  }
};


FuncInstr::FuncInstr(uint32 bytes)
{
    this->initFormat(bytes);
    this->bytes.raw = bytes;
    switch (this->format)
    {
        case FORMAT_R:
            this->parseR(bytes);
            break;
        case FORMAT_I:
            this->parseI(bytes);
            break;
        case FORMAT_J:
            this->parseJ(bytes);
            break;
        default:
            assert(!"undknown format");
    }
}

std::string FuncInstr::Dump( std::string indent) const
{
    std::ostringstream oss;
    oss << indent;
    uint8 instrId;
    switch (this->format)
    {
        case FORMAT_R:
            instrId = getISAFromTable(this->bytes.asR.opcode, this->bytes.asR.funct);
            switch(isaTable[instrId].type) {
                case FuncInstr::ADD:
                case FuncInstr::SUB:
                    oss << getISAname(instrId) << " "
                        << getREGname(this->bytes.asR.d) << ", "
                        << getREGname(this->bytes.asR.s) << ", "
                        << getREGname(this->bytes.asR.t) << endl;
                    break;
                case FuncInstr::SHIFT:
                    oss << getISAname(instrId) << " "
                        << getREGname(this->bytes.asR.d) << ", "
                        << getREGname(this->bytes.asR.t) << ", "
                        << getREGname(this->bytes.asR.S) << endl;
                    break;
                case FuncInstr::JUMP:
                    oss << getISAname(instrId) << " "
                        << getREGname(this->bytes.asR.s) << endl;
                    break;
                default:
                    assert(!"undefined type of instruction");
            }
            break;
        case FORMAT_I:
            instrId = getISAFromTable(this->bytes.asI.opcode);
            switch(isaTable[instrId].type) {
                case ADD:
                    oss << getISAname(instrId) << " "
                        << getREGname(this->bytes.asI.t) << ", "
                        << getREGname(this->bytes.asI.s) << ", "
                        << this->bytes.asI.imm << endl;
                    break;
                case BRANCH:
                    oss << getISAname(instrId) << " "
                        << getREGname(this->bytes.asI.s) << ", "
                        << getREGname(this->bytes.asI.t) << ", "
                        << this->bytes.asI.imm << endl;
                    break;
            }
            break;
        case FORMAT_J:
            instrId = getISAFromTable(this->bytes.asJ.opcode);
            oss << getISAname(instrId) << " "
                << "0x" << hex << this->bytes.asJ.addres << dec << endl;
            break;
        default:
            assert(!"Invalid command format");
    }
    return oss.str();
}

void FuncInstr::initFormat( uint32 bytes)
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

void FuncInstr::parseR(uint32 bytes)
{
    this->bytes.raw = bytes;
    this->dReg = this->bytes.asR.d;
    this->tReg = this->bytes.asR.t;
    this->sReg = this->bytes.asR.s;
    this->imm  = this->bytes.asR.S;
}

void FuncInstr::parseI(uint32 bytes)
{
    this->bytes.raw = bytes;
    this->tReg = this->bytes.asI.t;
    this->sReg = this->bytes.asI.s;
    this->imm  = this->bytes.asI.imm;
}

void FuncInstr::parseJ(uint32 bytes)
{
    this->bytes.raw = bytes;
    this->addr = this->bytes.asJ.addres;
}


uint8 FuncInstr::getISAFromTable(const uint8 opcode, const uint8 func) const
{
    for(int i = 0; i < ISA_TABLE_SIZE; i++)
    {
        if(isaTable[i].opcode == opcode && isaTable[i].func == func )
        {
            return i;
        }
    }
    assert(!"Instruction not found");
    return (uint32)-1;
}

const char *FuncInstr::getISAname(const uint8 id) const
{
    if(id < ISA_TABLE_SIZE ) {
        return isaTable[id].name;
    }
    assert(!"Instruction not found");
    return NULL;
}

const string FuncInstr::getREGname(const uint8 reg) const
{
    ostringstream oss;
    if(reg == 0) {
        return string("$zero");
    }
    if(reg == 1)
        return string("$at");
    if(2 <= reg && reg <=3)
    {
        oss << "$v" << (reg - 2);
        return oss.str();
    }
    if(4 <= reg && reg <= 7)
    {
        oss << "$a" << reg - 4;
        return oss.str();
    }
    if(8 <= reg && reg <= 15)
    {
        oss << "$t" << (reg - 8);
        return oss.str();
    }
    if(16 <= reg && reg <= 25)
    {
        oss << "$s" << (reg - 16);
        return oss.str();
    }
    if(26 <= reg && reg <= 27)
    {
        oss << "$k" << (reg - 26);
        return oss.str();
    }
    if(reg == 28)
        return string("$gp");
    if(reg == 29)
        return string("$sp");
    if(reg == 30)
        return string("$fp");
    if(reg == 31)
        return string("$ra");
    assert(!"undknown register");
    return NULL;
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump("");
}