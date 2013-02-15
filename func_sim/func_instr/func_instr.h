/**
 * func_instr.h - Header of module implementing instruction 
 * decoder and disassembler.
 * 
 */


//protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <string>

//uArchSim modules
#include <types.h>

using namespace std;

struct InstrTempl; //prototype, will be described below

class FuncInstr
{
    uint8 opcode, funct;
    uint32 shmt, imm, addr;
       
    enum reg_addr { ZERO, AT, V0, V1, A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5, T6, T7, S0, S1, S2, S3,
    S4, S5, S6, S7, T8, T9, K0, K1, GP, SP, FP, RA, empty};

    reg_addr rs, rd, rt;
    string instr_output;
    public:
        FuncInstr ( uint32 bytes);
        enum form_type { R_TYPE, I_TYPE, J_TYPE} fType;
        enum instr_type { 
            ADD, 
            ADDU, 
            SUB, 
            SUBU, 
            ADDI, 
            ADDIU, 
            SLL, 
            SRL,
            BEQ, 
            BNE, 
            J, 
            JR, 
            MOVE, 
            CLEAR, 
            NOP} 
        InstrName;
	    std::string Dump( std::string indent = " ") const;
        struct InstrTempl Curr_Instr;
        struct InstrTempl match_to_ISA();
        string reg_out( reg_addr reg);
};

struct InstrTempl
{
    string instr_out_name;
    uint8 instr_opcode;
    uint8 instr_funct;
    FuncInstr::fType form;
    FuncInstr::instr_type name;

    bool usage_funct;
    bool usage_rs;
    bool usage_rt;
    bool usage_rd;
    bool usage_shmt;
    bool usage_imm;
    bool usage_addr;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);
#endif 
