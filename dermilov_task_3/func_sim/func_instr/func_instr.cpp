/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */


#include <func_instr.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name    opcode   func   format    operation      function pointer
    { "add",      0x0,  0x20,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::add},
    { "addu",     0x0,  0x21,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::add},
    { "sub",      0x0,  0x22,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::sub},
    { "subu",     0x0,  0x23,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::sub},
    { "addi",     0x8,  0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::add_imm},
    { "addiu",    0x9,  0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::add_imm},

    { "mult",     0x0,  0x18,  FORMAT_R, OUT_R_MULTIP,  &FuncInstr::mul},
    { "multu",    0x0,  0x19,  FORMAT_R, OUT_R_MULTIP,  &FuncInstr::mul},
    { "div",      0x0,  0x1A,  FORMAT_R, OUT_R_MULTIP,  &FuncInstr::divu},
    { "divu",     0x0,  0x1B,  FORMAT_R, OUT_R_MULTIP,  &FuncInstr::divu},
    { "mfhi",     0x0,  0x10,  FORMAT_R, OUT_R_MOVE,    &FuncInstr::movh},
    { "mthi",     0x0,  0x11,  FORMAT_R, OUT_R_MOVE,    &FuncInstr::movh},
    { "mflo",     0x0,  0x12,  FORMAT_R, OUT_R_MOVE,    &FuncInstr::movl},
    { "mtlo",     0x0,  0x13,  FORMAT_R, OUT_R_MOVE,    &FuncInstr::movl},

    { "sll",      0x0,  0x0,   FORMAT_R, OUT_R_SHAMT,   &FuncInstr::shift_l},
    { "srl",      0x0,  0x2,   FORMAT_R, OUT_R_SHAMT,   &FuncInstr::shift_r},
    { "sra",      0x0,  0x3,   FORMAT_R, OUT_R_SHAMT,   &FuncInstr::shift},
    { "sllv",     0x0,  0x4,   FORMAT_R, OUT_R_LOGIC,   &FuncInstr::shift},
    { "srlv",     0x0,  0x6,   FORMAT_R, OUT_R_LOGIC,   &FuncInstr::shift},
    { "srav",     0x0,  0x7,   FORMAT_R, OUT_R_LOGIC,   &FuncInstr::shift},
    { "lui",      0xF,  0x0,   FORMAT_I, OUT_I_LUI,     &FuncInstr::load_up_imm},

    { "slt",      0x0,  0x2A,  FORMAT_R, OUT_R_LOGIC,   &FuncInstr::set},
    { "sltu",     0x0,  0x2B,  FORMAT_R, OUT_R_LOGIC,   &FuncInstr::set},
    { "slti",     0xA,  0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::set},
    { "sltiu",    0xB,  0x0,   FORMAT_I, OUT_I_ARITHM,  &FuncInstr::set},

    { "and",      0x0,  0x24,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::and_},
    { "or" ,      0x0,  0x25,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::or_},
    { "xor",      0x0,  0x26,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::xor_},
    { "nor",      0x0,  0x27,  FORMAT_R, OUT_R_ARITHM,  &FuncInstr::nor},
    { "andi",     0xC,  0x0,   FORMAT_I, OUT_I_LOGIC,   &FuncInstr::andi},
    { "ori",      0xD,  0x0,   FORMAT_I, OUT_I_LOGIC,   &FuncInstr::ori},
    { "xori",     0xE,  0x0,   FORMAT_I, OUT_I_LOGIC,   &FuncInstr::xori},

    { "beq",      0x4,  0x0,   FORMAT_I, OUT_I_BRANCH,  &FuncInstr::beq},
    { "bne",      0x5,  0x0,   FORMAT_I, OUT_I_BRANCH,  &FuncInstr::bneq},
    { "blez",     0x6,  0x0,   FORMAT_I, OUT_I_CONDIT,  &FuncInstr::cond},
    { "bgtz",     0x7,  0x0,   FORMAT_I, OUT_I_CONDIT,  &FuncInstr::cond},

    { "j",        0x2,  0x0,   FORMAT_J, OUT_J_JUMP,    &FuncInstr::jump},
    { "jal",      0x3,  0x0,   FORMAT_J, OUT_J_JUMP,    &FuncInstr::jump_none},
    { "jr",       0x0,  0x8,   FORMAT_R, OUT_R_JUMP,    &FuncInstr::jump_reg},
    { "jalr",     0x0,  0x9,   FORMAT_R, OUT_R_JUMP,    &FuncInstr::jump_none},

    { "lb",       0x20, 0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::addr_estim},
    { "lh",       0x21, 0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::addr_estim},
    { "lw",       0x23, 0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::addr_estim},
    { "lbu",      0x24, 0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::addr_estim},
    { "lhu",      0x25, 0x0,   FORMAT_I, OUT_I_LOAD,    &FuncInstr::addr_estim},

    { "sb",       0x28, 0x0,   FORMAT_I, OUT_I_STORE,   &FuncInstr::addr_estim},
    { "sh",       0x29, 0x0,   FORMAT_I, OUT_I_STORE,   &FuncInstr::addr_estim},
    { "sw",       0x2b, 0x0,   FORMAT_I, OUT_I_STORE,   &FuncInstr::addr_estim},

    { "syscall",  0x0,  0xC,   FORMAT_R, OUT_R_SPECIAL, &FuncInstr::spec},
    { "break",    0x0,  0xD,   FORMAT_R, OUT_R_SPECIAL, &FuncInstr::spec},
    { "trap",     0x1A, 0x0,   FORMAT_J, OUT_J_JUMP,    &FuncInstr::jump_none}

};
const uint32 FuncInstr::isaTableSize = sizeof(isaTable) / sizeof(isaTable[0]);

const char *FuncInstr::regTable[] = 
{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", 
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra"
};

const FuncInstr::Store_Load FuncInstr::SLTable[] =
{
//    opcode  function ptr                byte(s)
    { 0x28,   &FuncInstr::mem_size8},//       1},
    { 0X29,   &FuncInstr::mem_size16},//      2},
    { 0x2b,   &FuncInstr::mem_size32},//      4},
    { 0x20,   &FuncInstr::mem_size8},//       1},
    { 0X21,   &FuncInstr::mem_size16},//      2},
    { 0x23,   &FuncInstr::mem_size32},//      4},
    { 0X24,   &FuncInstr::mem_size8},//       1},
    { 0x25,   &FuncInstr::mem_size16} //      2}
};

const uint32 FuncInstr::St_Ld_Size = 8;
FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr(bytes)//, _PC(PC) 
{
  // only find instruction number in the table and write registers' numbers into v_src1, v_src2, v_dst
    init_Format();
    switch ( format)
    {
        case FORMAT_R:
            init_R();
            break;
        case FORMAT_I:
            init_I();
            break;
        case FORMAT_J:
            init_J();
            break;
        case FORMAT_UNKNOWN:
            init_Unknown();
            break;
    }
    mem_addr = 0;
    new_PC = PC + 4;
    v_dst = 0;
    v_src1 = 0;
    v_src2 = 0;
}

std::string FuncInstr::Dump( std::string indent) const
{
    return indent + disasm;
}

void FuncInstr::init_Format()
{
    for ( size_t i = 0; i < isaTableSize; i++) {
        if ( instr.asR.opcode == isaTable[i].opcode)
        {
            format = isaTable[i].format;
            operation = isaTable[i].operation;
            isaNum = i;
            return;
        }
    }
    format = FORMAT_UNKNOWN;
}


void FuncInstr::init_R()
{
    // find instr by functor
    for (isaNum = 0; isaNum < isaTableSize; ++isaNum) {
        if (( instr.asR.opcode == isaTable[isaNum].opcode) &&
            ( instr.asR.funct == isaTable[isaNum].funct))
        {
            operation = isaTable[isaNum].operation;
            break;
        }
    }
    if ( isaNum == isaTableSize)     // if didn't found funct
    {
        format = FORMAT_UNKNOWN;
        return;
    }

    switch( operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_LOGIC:
            v_src1 = instr.asR.rt;
            v_src2 = instr.asR.rs;
            v_dst = instr.asR.rd;
            break;
        case OUT_R_SHAMT:
            v_src1 = instr.asR.rt;
            v_dst = instr.asR.rd;
            break;
        case OUT_R_JUMP:
            v_src2 = instr.asR.rs;
            break;
        default:
            break;
    }
}

void FuncInstr::init_I()
{
    switch( operation)
    {
        case OUT_I_ARITHM: 
        case OUT_I_BRANCH:
        case OUT_I_LOGIC:
        case OUT_I_LOAD:
        case OUT_I_STORE:
            v_src1 = instr.asI.rt;
            v_src2 = instr.asI.rs;
            break;
        case OUT_I_LUI:
            v_src1 = instr.asI.rt;
        default:
            break;
    }
}

void FuncInstr::execute()
{
    if ( check())
    {
        (this->*isaTable[isaNum].func_ptr)();
        /*void (  *fp)( void);
        fp = isaTable[ isaNum].func_ptr;
        ( *fp)();*/
    }
}

void FuncInstr::fit_mem_size()
{
    for ( uint32 i = 0; i < St_Ld_Size; ++i) 
    {
        if ( instr.asI.opcode == SLTable[i].opcode)
        {
            (this->*SLTable[i].func_ptr)();
            /*void ( FuncInstr:: *fp)( void);
            fp = SLTable[i].func_ptr;
            ( *fp)();*/
            break;
        }
    }
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

void FuncInstr::dump_R()
{
    ostringstream oss;
    oss << isaTable[isaNum].name;
    switch( operation)
    {
        case OUT_R_ARITHM:
            oss << " $"                                                              \
            << regTable[instr.asR.rd] << " [0x" << hex << v_dst << dec << "], $"     \
            << regTable[instr.asR.rs] << " [0x" << hex << v_src2 << dec << "], $"    \
            << regTable[instr.asR.rt] << " [0x" << hex << v_src1 << dec << "]"; 
            break;
        case OUT_R_SHAMT:
            oss << " $"                                                              \
            << regTable[instr.asR.rd] << " [0x" << hex << v_dst << dec << "], $"     \
            << regTable[instr.asR.rt] << " [0x" << hex << v_src2 << dec << "],"      \
            << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << " $"                                                              \
            << regTable[instr.asR.rs] << " [0x" << hex << v_src1 << dec << "]";
            break;
        case OUT_R_LOGIC:
            oss << " $"
            << regTable[instr.asR.rd] << " [0x" << hex << v_dst << dec << "], $"     \
            << regTable[instr.asR.rt] << " [0x" << hex << v_src1 << dec << "], $"    \
            << regTable[instr.asR.rs] << " [0x" << hex << v_src2 << dec << "]"; 
        case OUT_R_MULTIP:
            oss << " $" << regTable[instr.asR.rs] << ", $" \
                << regTable[instr.asR.rt];
            break;
        case OUT_R_MOVE:
            oss << " $" << regTable[instr.asR.rd];
            break;
        case OUT_R_SPECIAL:
            break;
        default:
            break;
    }
    disasm = oss.str();
}

void FuncInstr::dump_I()
{
    std::ostringstream oss;
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            oss                                                                     \
            << regTable[instr.asI.rt] << " [0x" << hex << v_src1 << dec << "], $"   \
            << regTable[instr.asI.rs] << " [0x" << hex << v_src2 << dec << "], "    \
            << std::hex << "0x" << static_cast< signed int>( instr.asI.imm)         \
            << std::dec;                                                            
            break;
        case OUT_I_BRANCH:
        case OUT_I_LOGIC:
            oss                                                                     \
            << regTable[instr.asI.rs] << " [0x" << hex << v_src2 << dec << "], $"   \
            << regTable[instr.asI.rt] << " [0x" << hex << v_src1 << dec << "], "    \
            << std::hex << "0x" << static_cast< signed int>( instr.asI.imm)         \
            << std::dec;
            break;
        case OUT_I_LOAD:
        case OUT_I_STORE:
            oss                                                                     \
            << regTable[instr.asI.rt] << " [0x" << hex << v_src1 << dec << "], "    \
            << std::hex << "0x" << static_cast< signed int>( instr.asI.imm)         \
            << std::dec << "(&"                                                     \
            << regTable[instr.asI.rt] << " [0x" << hex << v_src1 << dec << "]) ";   \
            break;
        case OUT_I_CONDIT:
            oss                                                                     \
            << regTable[instr.asI.rs] << " [0x" << hex << v_src2 << dec << "], "    \
            << std::hex << "0x" << static_cast< signed int>( instr.asI.imm)         \
            << std::dec;                                                            
            break;
        case OUT_I_LUI:
            oss                                                                     \
            << regTable[instr.asI.rt] << " [0x" << hex << v_src1 << dec << "], "    \
            << std::hex << "0x" << static_cast< signed int>( instr.asI.imm)         \
            << std::dec;
            break;
        default:
            break;
    }
    disasm = oss.str();
}

void FuncInstr::dump_J()
{
    std::ostringstream oss;
    oss << isaTable[isaNum].name << " "
        << std::hex << "0x" <<instr.asJ.imm;
    disasm = oss.str();
}

void FuncInstr::dump_Unknown()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown" << std::endl;
    disasm = oss.str();
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}
