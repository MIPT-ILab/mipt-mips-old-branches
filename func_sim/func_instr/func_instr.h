/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */


#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

enum RegNum
{
    ZERO,
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
    MAX_REG
};

class FuncInstr
{
    public:
        const uint32 PC;

        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;

        uint32 mem_addr;
        uint32 mem_num_bytes;
        uint32 new_PC;

        bool is_load;

        FuncInstr(uint32 bytes, uint32 _PC = 0);
        std::string Dump( std::string indent = " ") const;

        RegNum get_src1_num_index() const;
        RegNum get_src2_num_index() const;
        RegNum get_dst_num_index() const;

        void execute();

    private:
        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        } format;

        enum OperationType
        {
            OUT_R_ARITHM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_J_JUMP,
            OUT_I_SHAMT,
            LOGICAL_R,
            LOGICAL_I,
            LOAD,
            STORE
        } operation;

        union _instr
        {
            struct
            {
                unsigned funct  :6;
                unsigned shamt  :5;
                unsigned rd     :5;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asR;
            struct
            {
                unsigned imm    :16;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asI;
            struct
            {
                unsigned imm    :26;
                unsigned opcode :6;
            } asJ;
            uint32 raw;

            _instr(uint32 bytes) {
                 raw = bytes;
            }
        } instr;

        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OperationType operation;

            void (FuncInstr::*method)();
        };
        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];

        std::string disasm;
                                                               
        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();

        //operations' methods:
        inline void add() { v_dst = v_src2 + v_src1; }
        inline void sub() { v_dst = v_src2 - v_src1; }
        inline void addi() { v_dst = v_src1 + instr.asI.imm; }
        
        inline void  left_shift() { v_dst = v_src1 << instr.asR.shamt; }
        inline void right_shift() { v_dst = v_src1 >> instr.asR.shamt; }
        inline void left_shift_16() { v_dst = instr.asI.imm << 16; }
        
        inline void bit_and() { v_dst = v_src1 & v_src2; }
        inline void bit_or()  { v_dst = v_src1 | v_src2; }
        inline void bit_xor() { v_dst = v_src1 ^ v_src2; }
        inline void bit_andi() { v_dst = v_src1 & instr.asI.imm; }
        inline void bit_ori()  { v_dst = v_src1 | instr.asI.imm; }
        inline void bit_xori() { v_dst = v_src1 ^ instr.asI.imm; }

        inline void beq() { if (v_src1 == v_src2) new_PC += (instr.asI.imm << 2); }
        inline void bne() { if (v_src1 != v_src2) new_PC += (instr.asI.imm << 2); }
        
        inline void jump()  { new_PC = (PC & 0xf0000000) | (instr.asJ.imm << 2); }
        inline void jumpr() { new_PC = v_src1; }
        
        void load8(); 
        void load16();
        void load32();

        void store8();
        void store16();
        void store32();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H