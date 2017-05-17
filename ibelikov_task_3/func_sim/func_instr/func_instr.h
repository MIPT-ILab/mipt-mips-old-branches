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

class MIPS;

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>
#include <func_sim.h>

//class MIPS;

class FuncInstr
{
    friend class MIPS;

    public:

        int get_dst_num_index()  const;

        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;
        uint32 mem_addr;
        uint32 v_shamt;
        uint32 jump_address;
        uint32 new_PC;
        uint32 PC;

        enum memsize{
            BYTE,
            HWORD,
            WORD,
            UBYTE,
            UHWORD,
            NOT_LDST
        };

        FuncInstr( uint32 bytes, uint32 PC);
        std::string Dump( std::string indent = " ") const;

        void execute();
        memsize get_ldst_size() const   { return isaTable[ isaNum].ldst_size;               }
        bool is_jump()  const { return  ( this->format == FORMAT_J ? true : false);         }
        bool is_load()  const { return  ( this->operation == OUT_I_LOAD ? true : false);    }
        bool is_store() const { return  ( this->operation == OUT_I_STORE ? true : false);   }

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
            OUT_I_SHIFT,
            OUT_I_LOG,
            OUT_I_LOAD,
            OUT_I_STORE,
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

            memsize ldst_size;

            void (FuncInstr::*func)(void);
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

        void add()  { v_dst = v_src1 + v_src2;  }
        void sub()  { v_dst = v_src1 - v_src2;  }
        void mul()  { v_dst = v_src1 * v_src2;  }
        void addi() { v_dst = v_src2 + mem_addr;}

        void beq()  { if ( v_src1 == v_src2) new_PC += ( mem_addr << 2); }
        void bne()  { if ( v_src1 != v_src2) new_PC += ( mem_addr << 2); }
        void j()    { new_PC = (PC & 0xf0000000) | (jump_address << 2); }
        void jr()   { new_PC = v_src1; }

        void sll()  { v_dst = v_src1 << v_src2; }
        void srl()  { v_dst = v_src1 >> v_src2; }
        void slli() { v_dst = v_src2 << v_shamt;}
        void srli() { v_dst = v_src2 >> v_shamt;}
        void lui()  { v_dst = mem_addr << 16;   }

        void _and() { v_dst = v_src1 & v_src2;  }
        void _or()  { v_dst = v_src1 | v_src2;  }
        void _xor() { v_dst = v_src1 ^ v_src2;  }
        void andi() { v_dst = v_src1 & mem_addr;}
        void ori()  { v_dst = v_src1 | mem_addr;}
        void xori() { v_dst = v_src1 ^ mem_addr;}

        void nop()  { }

        void sb()   { v_dst = v_src2;   }
        void sh()   { v_dst = v_src2;   }
        void sw()   { v_dst = v_src2;   }
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H
