/***************************************************************

    The implementation of instruction decoder

    @date: December 3, 2014
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @version: 1.0 (December 3, 2014) 

***************************************************************/

// Generic C
#include <string.h>

// Generic C++
#include <sstream>
#include <bitset>

// uArchSim modules
#include <func_instr.h>

uint32 get_instr(uint8 *where)
{
    ASSERT(where != NULL, "bad address");

    uint32 instr = 0;
    for(uint8 i = 0; i < 4; i++)
        instr |= where[i] << 8 * (3 - i);

    return instr;
}

FuncInstr::FuncInstr(uint32 bytes)
{
    cout << bitset<32>(bytes) << endl;
    instr.raw = bytes;

    initFormat(bytes);

    switch (format) {
    case R:
        parseR(bytes);
        break;
    case I:
        parseI(bytes);
        break;
    case J:
        parseJ(bytes);
        break;
    default:
        ASSERT(0, "ERROR.*");
    }
};

void FuncInstr::initFormat(uint32 bytes)
{
    switch (instr.R.opcode) {
    case 0:
        format = R;
        break;
    case 8: case 9: case 4: case 5:
        format = I;
        break;
    case 2:
        format = J;
        break;
    default:
        format = UNEXP;
    }
}

void FuncInstr::parseR(uint32 bytes)
{
    switch (instr.R.funct) {
    case 32:
        type = ADD;
        name = "add";
        break;
    case 33:
        type = ADD;
        name = "addu";
        break;
    case 34:
        type = SUB;
        name = "sub";
        break;
    case 35:
        type = SUB;
        name = "subu";
        break;
    case 0:
        type = SHIFT;
        name = "sll";
        break;
    case 2:
        type = SHIFT;
        name = "srl";
        break;
    case 8:
        type = JUMP;
        name = "jr";
        break;
    default:
        ASSERT(0, "bad function type");
    }
}

void FuncInstr::parseI(uint32 bytes)
{
    switch (instr.I.opcode) {
    case 8:
        type = ADD;
        name = "addi";
        break;
    case 9:
        type = ADD;
        name = "addiu";
        break;
    case 4:
        type = BRNCH;
        name = "beq";
        break;
    case 5:
        type = BRNCH;
        name = "bne";
        break;
    default:
        ASSERT(0, "bad operation code");
    }
}

void FuncInstr::parseJ(uint32 bytes)
{
    type = JUMP;
    name = "j";
}

const char *FuncInstr::get_name(REGTYPE type) const
{
    uint8 bits; 
    switch (type) {
    case S:
        bits = instr.R.s;
        break;
    case T:
        bits = instr.R.t;
        break;
    case D:
        bits = instr.R.d;
        break;
    default:
        ASSERT(0, "wrong type");
    }
    
    switch (bits) {
    case ZERO: return "zero";
    case AT:   return "at";
    case T0:   return "t0";
    case T1:   return "t1";
    case T2:   return "t2";
    case T3:   return "t3";
    case T4:   return "t4";
    case T5:   return "t5";
    case T6:   return "t6";
    case T7:   return "t7";
    case T8:   return "t8";
    case T9:   return "t9";
    default: ASSERT(0, "unexpected register");
    }
}

string FuncInstr::Dump(string indent) const
{
    ostringstream oss;

    oss << indent << name << hex;
    switch (format) {
    case R:
        switch (type) {
        case ADD: case SUB:
        oss << indent
            << " $" << get_name(D) << ","
            << " $" << get_name(S) << ","
            << " $" << get_name(T);
            break;
        case SHIFT:
            oss << indent
            << " $" << get_name(D) << ","
            << " $" << get_name(T) << ","
            << " 0x" << instr.R.d;
            break;
        case JUMP:
            oss << indent << " 0x" << get_name(S);
            break;
        }
        break;
    case I:
        switch (type) {
        case ADD:
            oss << " $" << get_name(T) << ","
                << " $" << get_name(S) << ","
                << " 0x" << instr.I.imm;
            break;
        case BRNCH:
            oss << " $" << get_name(S) << ","
                << " $" << get_name(T) << ","
                << " 0x" << instr.I.imm;
            break;
        }
        break;
    case J:
        oss << " 0x" << instr.J.addr;
        break;
    default:
        ASSERT(0, "unexpected format");
    } 

    return oss.str();
}

string InstrList::Dump(string indent) const
{
    ostringstream oss;
    
    oss << "section \"" << section_name << "\":" << endl;

    for (vector<FuncInstr>::const_iterator it = isaTable.begin();
         it != isaTable.end(); ++it)
        oss << it->Dump(indent) << endl;

    return oss.str();
}
