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

class FuncInstr
{
    public:
        FuncInstr( uint32 bytes, uint32 PC = 0);
        std::string Dump( std::string indent = " ") const;

	uint32 v_src1;
	uint32 v_src2;
	uint32 v_dst;
	uint32 mem_addr;
    uint32 bytes;
    bull ld;
    uint32 v_dst;
	uint32 new_PC;
	int get_src1_num_index() const;
	int get_src2_num_index() const;
    int get_dst_num_index() const;


    private:
        void add();
	    void sub();
        void mul();
	    void div();
	    void execute(); 
	
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
            OUT_R_LOGICAL,
            OUT_I_LOAD,
            OUT_I_LOGICAL,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_J_JUMP,
            OUT_I_STORE
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

};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H

