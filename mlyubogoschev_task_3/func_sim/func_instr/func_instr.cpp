/**
 * func_instr.cpp - The methods for implementing the concept of
 * disassembler of MIPS ISA.
 * @author Mikhail Lyubogoschev <lyubogoshchev@phystech.edu>
 * Copyright 2014 uArchSim iLab project
 */

#include <func_instr.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    //name  opcode func  format               type              comm_type
    { "add ", 0x0, 0x20, FuncInstr::FORMAT_R, FuncInstr::ADD, FuncInstr::R_ARITHM },
    { "addu ", 0x0, 0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU, FuncInstr::R_ARITHM },
    { "sub ", 0x0, 0x22, FuncInstr::FORMAT_R, FuncInstr::SUB, FuncInstr::R_ARITHM },
    { "subu ", 0x0, 0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU, FuncInstr::R_ARITHM },
    { "addi ", 0x8, 0, FuncInstr::FORMAT_I, FuncInstr::ADDI, FuncInstr::I_ARITHM },
    { "addiu ", 0x9, 0, FuncInstr::FORMAT_I, FuncInstr::ADDIU, FuncInstr::I_ARITHM },
    { "sll ", 0x0, 0x0, FuncInstr::FORMAT_R, FuncInstr::SLL, FuncInstr::R_ARITHM },
    { "srl ", 0x0, 0x2, FuncInstr::FORMAT_R, FuncInstr::SRL, FuncInstr::R_ARITHM },
    { "lui ", 0xf, 0, FuncInstr::FORMAT_I, FuncInstr::LUI, FuncInstr::I_LUI },
    { "and ", 0x0, 0x24, FuncInstr::FORMAT_R, FuncInstr::AND, FuncInstr::R_ARITHM},
    { "or ", 0x0, 0x25, FuncInstr::FORMAT_R, FuncInstr::OR, FuncInstr::R_ARITHM},
    { "xor ", 0x0, 0x26, FuncInstr::FORMAT_R, FuncInstr::XOR, FuncInstr::R_ARITHM},
    { "andi ", 0xc, 0, FuncInstr::FORMAT_I, FuncInstr::ANDI, FuncInstr::I_ARITHM},
    { "ori ", 0xd, 0, FuncInstr::FORMAT_I, FuncInstr::ORI, FuncInstr::I_ARITHM},
    { "xori ", 0xe, 0, FuncInstr::FORMAT_I, FuncInstr::XORI, FuncInstr::I_ARITHM},
    { "beq ", 0x4, 0, FuncInstr::FORMAT_I, FuncInstr::BEQ, FuncInstr::I_BRANCH },
    { "bne ", 0x5, 0, FuncInstr::FORMAT_I, FuncInstr::BNE, FuncInstr::I_BRANCH },
    { "j ", 0x2, 0, FuncInstr::FORMAT_J, FuncInstr::J, FuncInstr::J_JUMP },
    { "jr ", 0x0, 0x8, FuncInstr::FORMAT_R, FuncInstr::JR, FuncInstr::R_JUMP },
    { "lb ", 0x20, 0, FuncInstr::FORMAT_I, FuncInstr::LB, FuncInstr::I_MEMORY },
	{ "lh ", 0x21, 0, FuncInstr::FORMAT_I, FuncInstr::LH, FuncInstr::I_MEMORY },
    { "lw ", 0x23, 0, FuncInstr::FORMAT_I, FuncInstr::LW, FuncInstr::I_MEMORY },
	{ "lbu ", 0x24, 0, FuncInstr::FORMAT_I, FuncInstr::LBU, FuncInstr::I_MEMORY },
	{ "lhu ", 0x25, 0, FuncInstr::FORMAT_I, FuncInstr::LHU, FuncInstr::I_MEMORY },
    { "sb ", 0x28, 0, FuncInstr::FORMAT_I, FuncInstr::SB, FuncInstr::I_MEMORY },
	{ "sh ", 0x29, 0, FuncInstr::FORMAT_I, FuncInstr::SH, FuncInstr::I_MEMORY },
    { "sw ", 0x2b, 0, FuncInstr::FORMAT_I, FuncInstr::SW, FuncInstr::I_MEMORY }
};
const FuncInstr::Reg FuncInstr::regTable[] =
{
    { "$zero", 0 },
    { "$at", 1 },
    { "$v0", 2},
    { "$v1", 3},
    { "$a0", 4},
    { "$a1", 5},
    { "$a2", 6},
    { "$a3", 7},
    { "$t0", 8},
    { "$t1", 9},
    { "$t2", 10},
    { "$t3", 11},
    { "$t4", 12},
    { "$t5", 13},
    { "$t6", 14},
    { "$t7", 15},
    { "$s0", 16},
    { "$s1", 17},
    { "$s2", 18},
    { "$s3", 19},
    { "$s4", 20},
    { "$s5", 21},
    { "$s6", 22},
    { "$s7", 23},
    { "$t8", 24},
    { "$t9", 25},
    { "$k0", 26},
    { "$k1", 27},
    { "$gp", 28},
    { "$sp", 29},
    { "$fp", 30},
    { "$ra", 31}
};

FuncInstr::FuncInstr( uint32 bytes):
    opcMask( 0xff << 26),
    funcMask( 0x3f),
    exe( NULL),
	v_src( 0),
	v_tgt( 0),
	v_dst( 0)
{
    if ( bytes == ~0ull)
        exit( EXIT_FAILURE);
    dumpstr << hex << showbase;
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

FuncInstr:: ~FuncInstr(){}


void FuncInstr::InitFormat( uint32 _bytes)
{
    this->bytes.raw = _bytes;
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
    if ( bytes.asR.funct != 0x8)
    {
        reg1 =  regTable[bytes.asR.d].name;
        reg2 = regTable[bytes.asR.s1].name;
        reg3 = regTable[bytes.asR.t].name;
        operation = R_ARITHM;
    } else {
        reg1 = regTable[bytes.asR.s1].name;
        operation = R_JUMP;
    }
    switch ( bytes.asR.funct)
    {
        case 0x20:
            name = isaTable[ADD].name;
            exe = &FuncInstr::add;
            break;
        case 0x21:
            name = isaTable[ADDU].name;
            exe = &FuncInstr::add;
            break;
        case 0x22:
            name = isaTable[SUB].name;
            exe = &FuncInstr::sub;
            break;
        case 0x23:
            name = isaTable[SUBU].name;
            exe = &FuncInstr::sub;
            break;
        case 0x0:
            name = isaTable[SLL].name;
            cnst = bytes.asR.s2;
            exe = &FuncInstr::sll;
            break;
        case 0x2:
            name = isaTable[SRL].name;
            cnst = bytes.asR.s2;
            exe = &FuncInstr::srl;
            break;
        case 0x24:
            name = isaTable[AND].name;
            exe = &FuncInstr::_and;
            break;
        case 0x25:
            name = isaTable[OR].name;
            exe = &FuncInstr::_or;
            break;
        case 0x26:
            name = isaTable[XOR].name;
            exe = &FuncInstr::_xor;
            break;
        case 0x8:
            name = isaTable[JR].name;
            exe = &FuncInstr::jr;
            break;
        default:
            cerr << "ERROR: wrong command R\n";
            exit( EXIT_FAILURE);
    }
}

void FuncInstr::ParseI()
{
    reg1 = regTable[bytes.asI.t].name;
    reg2 = regTable[bytes.asI.s].name;
    cnst = bytes.asI.imm;
    switch ( bytes.asI.opcode )
    {
        case 0x8:
            name = isaTable[ADDI].name;
            exe = &FuncInstr::addi;
            operation = I_ARITHM;
            break;
        case 0x9:
            name = isaTable[ADDIU].name;
            exe = &FuncInstr::addi;
            operation = I_ARITHM;
            break;
        case 0x4:
            name = isaTable[BEQ].name;
            exe = &FuncInstr::beq;
            operation = I_BRANCH;
            break;
		case 0x5:
			name = isaTable[BNE].name;
			exe = &FuncInstr::bne;
			operation = I_BRANCH;
			break;
        case 0xf:
            name = isaTable[LUI].name;
            exe = &FuncInstr::lui;
            operation = I_LUI;
            break;
        case 0xc:
            name = isaTable[ANDI].name;
            exe = &FuncInstr::andi;
            operation = I_ARITHM;
            break;
         case 0xd:
            name = isaTable[ORI].name;
            exe = &FuncInstr::ori;
            operation = I_ARITHM;
            break;
        case 0xe:
            name = isaTable[XORI].name;
            exe = &FuncInstr::xori;
            operation = I_ARITHM;
            break;
        case 0x20:
            name = isaTable[LB].name;
            exe = &FuncInstr::lb;
            operation = I_MEMORY;
            break;
		case 0x21:
			name = isaTable[LH].name;
			exe = &FuncInstr::lw;
			operation = I_MEMORY;
			break;
        case 0x23:
            name = isaTable[LW].name;
            exe = &FuncInstr::lw;
            operation = I_MEMORY;
            break;
		case 0x24:
			name = isaTable[LBU].name;
			exe = &FuncInstr::lb;
			operation = I_MEMORY;
			break;
		case 0x25:
			name = isaTable[LHU].name;
			exe = &FuncInstr::lw;
			operation = I_MEMORY;
			break;
        case 0x28:
            name = isaTable[SB].name;
            exe = &FuncInstr::sb;
            operation = I_MEMORY;
            break;
		case 0x29:
			name = isaTable[SH].name;
			exe = &FuncInstr::sw;
			operation = I_MEMORY;
			break;
        case 0x2b:
            name = isaTable[SW].name;
            exe = &FuncInstr::sw;
            operation = I_MEMORY;
            break;
        default:
            cerr << "ERROR: wrong command I\n";
            exit( EXIT_FAILURE);
    }
}

void FuncInstr::ParseJ()
{
	operation = J_JUMP;
    switch ( bytes.asJ.opcode )
    {
        case 0x2:
            name = isaTable[J].name;
            cnst = bytes.asJ.addr;			
            exe = NULL;
            break;
        default:
            cerr << "ERROR: wrong command J\n";
            exit( EXIT_FAILURE);
    }
}

inline std::string FuncInstr::Dump( std::string indent) 
{
    switch ( operation)
    {
        case R_ARITHM:
            dumpstr << name << reg1 << " [" << v_dst << "], " <<  reg2 << " [" << v_src << "], " << reg3 << " [" << v_tgt << "]";
            break;
        case I_ARITHM:
            dumpstr << name << reg1 << " [" << v_src << "], " <<  reg2 << " [" << v_tgt << "], " << cnst;
            break;
        case I_BRANCH:
            dumpstr << name << reg1 << " [" << v_src << "], " <<  reg2 << " [" << v_tgt << "], " << cnst;
            break;
        case R_JUMP:
            dumpstr << name << reg1 << " [" << v_src << "]";
            break;
        case J_JUMP:
            dumpstr << name << cnst;
            break;
        case I_MEMORY:
            dumpstr << name << reg1 << " [" << v_tgt << "], " << cnst << "(" << reg2 << " [" << v_src << "])";
            break;
		case I_LUI:
			dumpstr << name << reg1 << " [" << v_tgt << "], " << cnst << endl;
    }
    return indent + dumpstr.str();
}

std::ostream& operator << ( std::ostream& out, FuncInstr& instr)
{
    return out << instr.Dump("");
}


RegNum FuncInstr::get_src_num_index() const
{
    if (format != FORMAT_J)
        return (RegNum) bytes.asI.s;
    else
        return ZERO;
}


RegNum FuncInstr::get_tgt_num_index() const
{
    if ( format != FORMAT_J)
        return ( RegNum) bytes.asI.t;
    else
        return ZERO;
}

RegNum FuncInstr::get_dst_num_index() const
{
    if ( format != FORMAT_J)
        return ( RegNum) bytes.asR.d;
    else
        return ZERO;
}
