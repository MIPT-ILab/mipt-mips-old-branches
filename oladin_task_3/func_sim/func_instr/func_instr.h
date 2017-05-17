/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */

/** Edited by Ladin Oleg. */

#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

class FuncInstr
{
    public:
        /* Describes properties of each instruction (read-write access). */
        union
        {
            struct // by bits
            {
                unsigned rnw : 1; // is NOT write to register file
                unsigned mrs : 1; // is signed read
                unsigned mrt : 2; // type of mem read: 0 - lb, 1 - lh, 2 - lw
                unsigned mr  : 1; // is read from memory
                unsigned mwt : 2; // type of mem write: 0 - sb, 1 - sh, 2 - sw
                unsigned mw  : 1; // is write to memory
            } bit;
            uint8 num; // as number
        } prop;

        uint32 new_PC; // address of next instruction
        uint32 mem_addr; // address for memory access
        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;

        int get_src1_num_index() const;
        int get_src2_num_index() const;
        int get_dst_num_index() const;
        void execute(); // starts micromethod

        FuncInstr( uint32 bytes, uint32 PC = 0); // requaries current PC
        std::string Dump( std::string indent = " ") const;

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
            OUT_R_LOG, // and, or, xor
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_I_LOAD, // lui
            OUT_I_LOG, // andi, ori, xori
            OUT_I_MEM, // lb, lh, lw, lbu, lhu, sb, sh, sw
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

        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OperationType operation;

            void ( FuncInstr::*func)(); // micromethod pointer
            uint8 properties; // access properties byte for "prop" union
        };
        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];

        std::string disasm;
                                                               
        const uint32 PC; // programm counter of this instruction
        void ( FuncInstr::*func)(); // function to execute

        /*
         * Micromethods to each operation (by default "d" - v_dst, "s" -
         * v_src1, "t" - v_src2).
         */
        void add() { v_dst = v_src2 + v_src1; }
        void addu() { v_dst = v_src2 + v_src1; }
        void sub() { v_dst = v_src2 - v_src1; }
        void subu() { v_dst = v_src2 - v_src1; }
        void addi() { v_dst = v_src1 + v_src2; }
        void addiu() { v_dst = v_src1 + v_src2; }
        void sll() { v_dst = v_src2 << v_src1; }
        void srl() { v_dst = v_src2 >> v_src1; }
        void beq() { new_PC += ( v_dst << 2) * ( v_src1 == v_src2); }
        void bne() { new_PC += ( v_dst << 2) * ( v_src1 != v_src2); }
        void j() { new_PC = ( PC & 0xf0000000) | ( v_src1 << 2); }
        void jr() { new_PC = v_src1; }
        void lui() { v_dst = v_src1 << v_src2; }
        void and_() { v_dst = v_src1 & v_src2; }
        void or_() { v_dst = v_src1 | v_src2; }
        void xor_() { v_dst = v_src1 ^ v_src2; }
        void andi() { v_dst = v_src1 & v_src2; }
        void ori() { v_dst = v_src1 | v_src2; }
        void xori() { v_dst = v_src1 ^ v_src2; }
        void lb() { mem_addr = v_src1 + v_src2; }
        void lh() { mem_addr = v_src1 + v_src2; }
        void lw() { mem_addr = v_src1 + v_src2; }
        void lbu() { mem_addr = v_src1 + v_src2; }
        void lhu() { mem_addr = v_src1 + v_src2; }
        void sb() { mem_addr = v_src1 + v_src2; }
        void sh() { mem_addr = v_src1 + v_src2; }
        void sw() { mem_addr = v_src1 + v_src2; }

        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H

