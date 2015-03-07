/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 * 
 * Modified by Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * 2015 MIPT-MIPS
 */


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
        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;
        uint32 mem_addr;
        uint32 PC_delta;

        uint32 i_dst;
        uint32 i_src1;
        uint32 i_src2;
        
        uint32 jaddr;

        enum _type
        {
            BYTE,
            HWORD,
            WORD,
            UBYTE,
            UHWORD
        } type;

        enum LoadStore
        {
            nothing,
            load,
            store
        } memOp;

        FuncInstr( uint32 bytes); // READY
        std::string Dump( std::string indent = " ") const; // READY
        void execute(); // READY

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
            OUT_R_LOG,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_I_LOG,
            OUT_I_MEM,
            OUT_I_SHIFT,
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

            void ( FuncInstr::*function)();
        };
        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];

        std::string disasm;
                                                               
        void initFormat(); //READY
        void initR(); // READY
        void initI(); // READY
        void initJ(); // READY
        void initUnknown(); // READY

        std::string outSrc1(); // READY
        std::string outSrc2(); // READY
        std::string outDst(); // READY
        std::string outImm(); // READY
        void ( FuncInstr::*function)();

        void add();
        void addu();
        void sub();
        void subu();
        void addi();
        void addiu();

        void sll();
        void srl();
        void lui();

        void _and();
        void _or();
        void _xor();
        void andi();
        void ori();
        void xori();

        void beq();
        void bne();

        void j();
        void jr();

        void lb();
        void lh();
        void lw();
        void lbu();
        void lhu();

        void sb();
        void sh();
        void sw();

        void preDump();
        void DumpR();
        void DumpI();
        void DumpJ();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr); // READY

#endif //FUNC_INSTR_H

