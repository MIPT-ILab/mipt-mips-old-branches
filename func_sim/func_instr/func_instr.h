/**
 * func_instr.h - Header of module implementing the concept of 
 * disassembler of MIPS ISA.
 * @author Mikhail Lyubogoschev <lyubogoshchev@phystech.edu>
 * Copyright 2014 uArchSim iLab project
 */

#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

#include <sstream>
#include <iomanip>
#include <string>
#include <ostream>
#include <cassert>

#include <types.h>
#include <func_memory.h>
//#include <func_sim.h>
enum RegNum {
    ZERO = 0,
    AT,
    V0, V1,
    A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5, T6, T7,
    S0, S1, S2, S3, S4, S5, S6, S7,
    T8, T9,
    K0, K1,
    GP,
    SP,
    FP,
    RA,
    MAXREG
};



class FuncInstr
{
private:
    
    enum Type
    {
        ADD = 0,
        ADDU,
        SUB,
        SUBU,
        ADDI,
        ADDIU,
        SLL,
        SRL,
        LUI,
        AND,
        OR,
        XOR,
        ANDI,
        ORI,
        XORI,
        BEQ,
        BNE,
        J,
        JR,
        LB,
		LH,
        LW,
		LBU,
		LHU,
        SB,
		SH,
        SW
    };
    union
    {
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;
        struct
        {
            unsigned funct:6;
            unsigned s2:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s1:5;
            unsigned opcode:6;
        } asR;
        struct
        {
            unsigned addr:26;
            unsigned opcode:6;
        } asJ;
        uint32 raw;
    } bytes;



    void InitFormat( uint32 _bytes);
    void ParseR();
    void ParseI();
    void ParseJ();
    const uint32 opcMask;
    const uint32 funcMask;
    
    string name, reg1, reg2, reg3;
    
    std::ostringstream dumpstr;

public:
    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    };
    enum OpType
    {
        R_ARITHM,
        I_ARITHM,
        R_JUMP,
        J_JUMP,
        I_MEMORY,
        I_BRANCH,
        I_LUI
    } operation;
    FuncInstr( uint32 bytes);
    ~FuncInstr();
    void add() { v_dst = v_src + v_tgt;};
    void sub() { v_dst = v_tgt - v_src;};
    void addi() { v_tgt = v_src + cnst;};
    void sll() { v_dst = v_tgt << bytes.asR.s2;};
    void srl() { v_dst = v_tgt >> bytes.asR.s2;};
    void beq() { if ( v_src == v_tgt) PC_delta = ( ( int16)cnst << 2) + 4; 
				else PC_delta = 4;};
    void bne() { if (v_src != v_tgt) PC_delta = ( (int16)cnst << 2) + 4; 
				else PC_delta = 4;
				cout << "PC_delta = " << PC_delta << endl;};
    void lui() { v_tgt = ( bytes.asI.imm << 16);};
    void _and() { v_dst = v_src & v_tgt;};
    void _or() { v_dst = v_src | v_tgt;};
    void _xor() { v_dst = v_src ^ v_tgt;};
    void andi() { v_tgt = v_src & cnst;};
    void ori() { v_tgt = v_src | cnst;};
    void xori() { v_tgt = v_src ^ cnst;};
    void jr() { PC_delta = v_src;};
    void lb() { mem_addr = v_src + cnst;};
    void lw() { mem_addr = v_src + cnst;};
    void sb() { mem_addr = v_src + cnst;};
    void sw() { mem_addr = v_src + cnst;};
    void (FuncInstr::*exe) ();
    

    void execute(){ if ( exe != NULL) (this->*exe)();};
    struct ISAEntry
    {
        const char* name;
        uint8 opcode;
        uint8 funct;
    
        FuncInstr::Format format;
        FuncInstr::Type type;
        FuncInstr::OpType operation;
    };
    static const ISAEntry isaTable[];
    struct Reg
    {
        const char* name;
        uint8 num; 
    };
    static const Reg regTable[];
    Format getFormat(){ return format;};
    int getIOpcode(){ return bytes.asI.opcode;};
    RegNum get_src_num_index() const;
    RegNum get_tgt_num_index() const;
    RegNum get_dst_num_index() const;
    
    uint32 v_src;
    uint32 v_tgt;
    uint32 v_dst;
    uint32 mem_addr;
    int32 PC_delta;
    uint32 cnst; //const
    
    
    inline std::string Dump( std::string indent = " ");
private:
    Format format;
};

std::ostream& operator<<( std::ostream& out, FuncInstr& instr);
#endif
