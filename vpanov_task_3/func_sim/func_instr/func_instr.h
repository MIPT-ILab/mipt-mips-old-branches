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
#include <iostream>

class FuncInstr
{
    public:

        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;
        uint32 mem_addr;
        uint32 PC_delta;

        uint32 rs;
        uint32 rt;
        uint32 rd;
        uint32 shamt;
        uint32 imm;

        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
        void execute();



    private:
        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        } format;
    public:
        enum OperationType
        {
            OUT_R_ARITHM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_I_LA,
            OUT_I_LB,
            OUT_I_LW,
            OUT_I_SB,
            OUT_I_SW,
            OUT_J_JUMP
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
    private:
        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OperationType operation;
        };
        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];

        void (FuncInstr::*exec_operation)();

        std::string disasm;
                                                               
        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();

        void add() { v_dst = v_src1 + v_src2;}
        void sub() { v_dst = v_src1 - v_src2; }
        void sll() { v_dst = v_src1 << v_src2; }
        void srl() { v_dst = v_src1 >> v_src2; }
        void beq() {
            if(v_src1 == v_dst) {
                PC_delta = v_src2 << 2;
            }
        }

        void bne() {
            if(v_src1 != v_dst) {
                PC_delta = v_src2 << 2;
            }
        }
        void j() {
            mem_addr = v_src2 << 2;
        }
        void jr() {
            mem_addr = v_src2;
        }

        void la() {
            v_dst = v_src1 << 16;
        }

        void load() {
            mem_addr = (v_src1 + v_src2);
        }

        void store() {
            mem_addr = (v_dst + v_src1);
        }

        // ...
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H

