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

enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        };

enum OperationType
        {
            OUT_R_ARITHM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_R_MULTIP,
            OUT_R_MOVE,
            OUT_R_LOGIC,
            OUT_R_SPECIAL,
            OUT_I_ARITHM,
            OUT_I_LOGIC,
            OUT_I_BRANCH,
            OUT_I_CONDIT,
            OUT_I_LUI,
            OUT_I_LOAD,
            OUT_I_STORE,
            OUT_J_JUMP
        };
class FuncInstr
{
    friend class MIPS;

    public:
        FuncInstr( uint32 bytes, uint32 PC = 0);
        std::string Dump( std::string indent = " ") const;

//    private:
        /*enum Format
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
            OUT_R_MULTIP,
            OUT_R_MOVE,
            OUT_R_LOGIC,
            OUT_R_SPECIAL,
            OUT_I_ARITHM,
            OUT_I_LOGIC,
            OUT_I_BRANCH,
            OUT_I_CONDIT,
            OUT_I_LUI,
            OUT_I_LOAD,
            OUT_I_STORE,
            OUT_J_JUMP
        } operation;*/
private:
        enum Format format;
        enum OperationType operation;
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
            void ( FuncInstr:: *func_ptr)( void);
        };

        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];

        std::string disasm;

        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;
        uint32 mem_addr;
//        uint32 _PC
        uint32 new_PC;

        struct Store_Load
        {
            uint8 opcode;
            void ( FuncInstr:: *func_ptr)();
        };

        static const Store_Load SLTable[];
        static const uint32 St_Ld_Size;

        FuncInstr( uint32 bytes);
                                                             
/*        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();*/

        void init_Format();
        void init_R();
        void init_I();
        void init_J() { /*none*/;}  
        void init_Unknown(){ /*none*/;} 

        int get_src1_num_index() const { return ( int) v_src1; }
        int get_src2_num_index() const { return ( int) v_src2; }

        void fit_mem_size();
        void dump_R();
        void dump_I();
        void dump_J(); 
        void dump_Unknown();


        void add() { v_dst = v_src1 + v_src2; }       // the same for addu
        void sub() { v_dst = v_src1 - v_src2; }       // the same for subu
        void add_imm() { v_dst = v_src2 + instr.asI.imm; }  // the same for addiu
        void mul(){ /*none*/;} 
        void divu(){ /*none*/;} 
        void movh(){ /*none*/;} 
        void movl(){ /*none*/;} 
        void shift_l() { v_dst = v_src1 << instr.asR.shamt; }
        void shift_r() { v_dst = v_src1 >> instr.asR.shamt; }
        void shift(){ /*none*/;} 
        void load_up_imm() { v_src1 = instr.asI.imm << 16; }
        void set(){ /*none*/;} 
        void and_() { v_dst = v_src2 & v_src1; }
        void or_() { v_dst = v_src2 | v_src1; }
        void xor_() { v_dst = v_src2 ^ v_src1; }
        void andi() { v_src1 = v_src2 & instr.asI.imm; }
        void ori() { v_src1 = v_src2 | instr.asI.imm; }
        void xori() { v_src1 = v_src2 ^ instr.asI.imm; }
        void nor(){ /*none*/;} 
        void beq() { if ( v_src2 == v_src1)                                         \
                        new_PC += 4; new_PC += ( instr.asI.imm << 2); }
        void bneq() { if ( v_src2 != v_src1)                                        \
                         new_PC += 4; new_PC += ( instr.asI.imm << 2); }  
        void cond() { /*none*/ }           
        void jump() { new_PC = ( new_PC & 0xf0000000) | ( instr.asJ.imm << 2); }
        void jump_none() { /*none*/ }
        void jump_reg() { new_PC = v_src2; }
        void addr_estim() { mem_addr = v_src2 + instr.asI.imm; }   // for load and stire
        void mem_size8() { v_dst = ( uint8) ( v_dst & 0xff); }
        void mem_size16() { v_dst = ( uint16) ( v_dst & 0xffff); }
        void mem_size32() { /*none*/ }
        void spec(){ /*none*/;} 

        bool check() { return format != FORMAT_UNKNOWN; }
        void execute();

};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H