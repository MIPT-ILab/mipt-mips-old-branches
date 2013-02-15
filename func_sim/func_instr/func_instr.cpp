/**
 * func_instr.cpp - the module implementing
 * instruction decoder and disassembler.
 */

// Generic C
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>

//Generic C++
#include <sstream>
#include <string>
#include <iostream>

//uArchSim modules
#include <func_instr.h>

FuncInstr::FuncInstr ( uint32 bytes)
{
    instr_output.clear();
    opcode = bytes >> 26;
    funct = bytes & 03F;
    rs = ( reg_addr) ( bytes & 0x03E00000) >> 20;
    rt = ( reg_addr) ( bytes & 0x1F0000) >> 15;
    rd = ( reg_addr) ( bytes & 0x0F800) >> 10;
    shmt = ( bytes & 0x07C0) >> 5;
    imm = ( bytes & 0x0FFFF);
    addr = ( bytes & 0x07FFFFFF);
    fType = get_type( bytes);

    Curr_Instr = match_to_ISA();
    check_for_pseudo( Curr_Instr);
};


static const InstrTempl ISA[12] = {
    { "add", 0, 0x20, R_TYPE, ADD, 1, 1, 1, 1, 0, 0, 0},
    { "addu", 0, 0x21, R_TYPE, ADDU, 1, 1, 1, 1, 0, 0, 0},
    { "sub", 0, 0x22, R_TYPE, SUB, 1, 1, 1, 1, 0, 0, 0},
    { "subu", 0, 0x23, R_TYPE, SUBU, 1, 1, 1, 1, 0, 0, 0},
    { "addi", 8, 0x0, I_TYPE, ADDI, 0, 1, 1, 0, 0, 1, 0},
    { "addiu", 9, 0x0, I_TYPE, ADDIU, 0, 1, 1, 0, 0, 1, 0},
    { "sll", 0, 0x0, R_TYPE, SLL, 1, 0, 1, 1, 1, 0, 0},
    { "srl", 0, 0x2, R_TYPE, SRL, 1, 0, 1, 1, 1, 0, 0},
    { "beq", 4, 0x0, I_TYPE, BEQ, 0, 1, 1, 0, 0, 1, 0},
    { "bne", 5, 0x0, I_TYPE, BNE, 0, 1, 1, 0, 0, 1, 0},
    { "j", 2, 0x0, J_TYPE, J, 0, 0, 0, 0, 0, 0, 1},
    { "jr", 0, 0x8, R_TYPE, JR, 1, 1, 0, 0, 0, 0, 0},
};

static const InstrTempl pseudo_ISA[3] = {
    { "move", 8, 0x0, I_TYPE, MOVE, 0, 1, 1, 0, 0, 0, 0},
    { "clear", 0, 0x21, R_TYPE, CLEAR, 1, 0, 1, 0, 0, 0, 0},
    { "nop", 0, 0x0, R_TYPE, NOP, 1, 0, 0, 0, 0, 0, 0}
};

form_type FuncInstr::get_type ( uint32 bytes)
{
    switch ( opcode)
    {
        case 0: return R_TYPE;
        case 2: return J_TYPE;
        case 4:
        case 5:
        case 8:
        case 9: return I_TYPE;
        default: 
            cerr << "This instruction isn't supported yet" << endl;
            exit(-1);
    }
};

struct InstrTempl FuncInstr::match_to_ISA()
{
    int cnt;
    for ( cnt = 0; cnt<12; cnt++)
    {
        if ( opcode == ISA[cnt].instr_opcode)
            if ( fType == R_TYPE)
            {
                if ( funct == ISA[cnt].instr_funct) Curr_Instr = ISA[cnt];
                else continue;
            }
            Curr_Instr = ISA[cnt];
    }
};

void FuncInstr::check_for_pseudo( struct InstrTempl &Example)
{
    if ( Example.instr_out_name == "addiu")
    {
        if (imm == 0) Example = pseudo_ISA[0]; 
    }
    if ( Example.instr_out_name == "addu")
    {
        if (( rs == ZERO) && ( rd == ZERO)) Example = pseudo_ISA[1];
    }
    if ( Example.instr_out_name == "sll")
    {
        if (( rd == ZERO) && ( rt == ZERO) && ( imm == 0) Example = pseudo_ISA[2];
    }
};

//write the output of the registers

string FuncInstr::reg_out ( reg_addr reg)
{
    switch( reg)
    {
        case ZERO: return "$zero";
        case AT: return "$at";
        case V0: return "$v0";
        case V1: return "$v1";
        case A0: return "$a0";
        case A1: return "$a1";
        case A2: return "$a2";
        case A3: return "$a3";
        case T0: return "$t0";
        case T1: return "$t1";
        case T2: return "$t2";
        case T3: return "$t3";
        case T4: return "$t4";
        case T5: return "$t5";
        case T6: return "$t6";
        case T7: return "$t7";
        case S0: return "$s0";
        case S1: return "$s1";
        case S2: return "$s2";
        case S3: return "$s3";
        case S4: return "$s4";
        case S5: return "$s5";
        case S6: return "$s6";
        case S7: return "$s7";
        case T8: return "$t8";
        case T9: return "$t9";
        case K0: return "$k0";
        case K1: return "$k1";
        case GP: return "$gp";
        case SP: return "$sp";
        case FP: return "$fp";
        case RA: return "$ra";
        default:
            cerr << "error with register output has occured";
            exit (-1);
    }
};

std::string FuncInstr::Dump( string indent)
{
    if ( instr_output.empty())
    {    
    ostringstream oss;
    string sep_op = ", "
    oss << indent << Curr_Instr.instr_out_name << "    ";
    if (( Curr_Instr.form == R_TYPE) || ( Curr_Instr.form == J_TYPE))
    {
        if ( usage_rd) oss << reg_out( rd);
        if ( usage_rs) oss << sep_op << reg_out( rs);
        if ( usage_rt) oss << sep_op << reg_out( rt); 
        if ( usage_shmt) oss << sep_op << "0x" << hex << shmt;
        if ( usage_addr) oss << "0x" << hex << addr;
    }
    else if ( Curr_Instr.form == I_TYPE)
    {
        if ( Curr_Instr.instr_opcode <= 5)
        {
            if ( usage_rs) oss << reg_out( rs);
            if ( usage_rt) oss << sep_op << reg_out( rt);
            if ( usage_imm) oss << sep_op << "0x" << hex << imm;
        }
        else 
        {
            if ( usage_rt) oss << reg_out( rt);
            if ( usage_rs) oss << sep_op << reg_out( rs);
            if ( usage_imm) oss << sep_op << "0x" << hex << imm;
        }
    }
    instr_output.assign( oss.str());
    };
    return instr_output;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump( indent = " ");
    return out;
};
