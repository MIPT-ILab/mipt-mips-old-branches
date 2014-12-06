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

// uArchSim modules
#include <func_instr.h>

const char[][] FuncInstr::reg_names =
{
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9",
    "$k0", "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra"
};

const FuncInstr::ISAEntry[] FuncInstr::isaTable =
{
    // name   opcode  func     format          type        reg_num|const_num
    { "add",   0x0,   0x20, FuncInstr::R, FuncInstr::ADD,     3,       0    },
    { "addu",  0x0,   0x21, FuncInstr::R, FuncInstr::ADD,     3,       0    },
    { "sub",   0x0,   0x22, FuncInstr::R, FuncInstr::SUB,     3,       0    },
    { "subu",  0x0,   0x23, FuncInstr::R, FuncInstr::SUB,     3,       0    },
    { "addi",  0x8,   0x0,  FuncInstr::I, FuncInstr::ADD,     2,       1    },
    { "addiu", 0x9,   0x0,  FuncInstr::I, FuncInstr::ADD,     2,       1    },
    { "sll",   0x0,   0x0,  FuncInstr::R, FuncInstr::SHIFT,   2,       1    },
    { "srl",   0x0,   0x2,  FuncInstr::R, FuncInstr::SHIFT,   2,       1    },
    { "beq",   0x4,   0x0,  FuncInstr::I, FuncInstr::BRNCH,   2,       1    },
    { "bne",   0x5,   0x0,  FuncInstr::I, FuncInstr::BRNCH,   2,       1    },
    { "j",     0x2,   0x0,  FuncInstr::J, FuncInstr::JUMP,    0,       1    },
    { "jr",    0x0,   0x8,  FuncInstr::R, FuncInstr::JUMP,    1,       0    },
};

uint32 get_instr(uint8 *where)
{
    ASSERT(where != NULL, "bad address");

    uint32 instr = 0;
    for(uint8 i = 0; i < 4; i++)
        instr |= where[i] << 8 * (3 - i);

    return instr;
}

void print_sec(vector<ElfSection>::iterator it)
{
    ASSERT(!(it->size % 4), "not a full instruction");

    InstrList instr(it->name);

    for (uint64 i = 0; i < it->size / 4; i++)
        instr.add(get_instr(it->content + i * 4));
            
    cout << instr;
}

FuncInstr::FuncInstr(uint32 bytes)
{
    instr.raw = bytes;

    initFormat(bytes);

    switch (format) {
    case R:
        parseR(bytes); break;
    case I:
        parseI(bytes); break;
    case J:
        parseJ(bytes); break;
    default:
        ASSERT(0, "ERROR.*");
    }
};

void FuncInstr::initFormat(uint32 bytes)
{
    switch (instr.R.opcode) {
    case 0x0:
        format = R; break;
    case 0x8: case 0x9: case 0x4: case 0x5:
        format = I; break;
    case 0x2:
        format = J; break;
    default:
        format = UNEXP;
    }
}

void FuncInstr::parseR(uint32 bytes)
{
    switch (instr.R.funct) {
    case 0x20:
        inrst_feat = isaTable[0];  break;
    case 0x21:
        inrst_feat = isaTable[1];  break;
    case 0x22:
        inrst_feat = isaTable[2];  break;
    case 0x23:
        inrst_feat = isaTable[3];  break;
    case 0x0:
        inrst_feat = isaTable[6];  break;
    case 0x2:
        inrst_feat = isaTable[7];  break;
    case 0x8:
        inrst_feat = isaTable[11]; break;
    default:
        ASSERT(0, "bad function type");
    }

    switch (instr.R.funct) {
    case 0x20: case 0x21: case 0x22: case 0x23:
        regist.push_back(reg_names[instr.R.d]);
        regist.push_back(reg_names[instr.R.s]);
        regist.push_back(reg_names[instr.R.t]);
        break;
    case 0x0: case 0x2:
        regist.push_back(reg_names[instr.R.d]);
        regist.push_back(reg_names[instr.R.t]);
        const_val = instr.R.shamt;
        break;
    case 0x8:
        regist.push_back(reg_names[instr.R.s]);
    }
}

void FuncInstr::parseI(uint32 bytes)
{
    switch (instr.I.opcode) {
    case 0x8:
        inrst_feat = isaTable[4]; break;
    case 0x9:
        inrst_feat = isaTable[5]; break;
    case 0x4:
        inrst_feat = isaTable[8]; break;
    case 0x5:
        inrst_feat = isaTable[9]; break;
    default:
        ASSERT(0, "bad operation code");
    }

    switch (instr.I.opcode) {
    case 0x8: case 0x9:
        regist.push_back(reg_names[instr.I.t]);
        regist.push_back(reg_names[instr.I.s]);
        break;
    case 0x4: case 0x5:
        regist.push_back(reg_names[instr.I.s]);
        regist.push_back(reg_names[instr.I.t]);
        break;
    }

    const_val = instr.I.imm;
}

void FuncInstr::parseJ(uint32 bytes)
{
    inrst_feat = isaTable[10];
    const_val = instr.J.addr;
}

string FuncInstr::Dump(string indent) const
{
    ostringstream oss;

    oss << indent << instr_feat.name;
    
    for (uint8 i = 0; i < instr_feat.reg_num; i++) {
        oss << " $" << regist[i];
        if (i != 2) oss << ",";
    }

    if (instr_feat.const_num)
        oss << " 0x" << hex << const_val;

    return oss.str();
}

string InstrList::Dump(string indent) const
{
    ostringstream oss;
    
    oss << "section \"" << section_name << "\":" << endl;

    for (vector<FuncInstr>::const_iterator it = list.begin();
         it != list.end(); ++it)
        oss << it->Dump(indent) << endl;

    return oss.str();
}
