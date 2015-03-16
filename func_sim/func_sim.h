
#ifndef FUNC_SIM_H
#define FUNC_SIM_H


#include <types.h>
#include <func_memory.h>
#include <func_instr.h>

enum RegNum {
    $zero,
    $at,
    $v0, $v1,
    $a0, $a1, $a2, $a3,
    $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7,
    $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7,
    $t8, $t9,
    $k0, $k1,
    $gp,
    $sp,
    $fp,
    $ra,
    MAX_REG
};

class FuncInstr;

class RF {
        uint32 array[MAX_REG];
    public:
        RF();
        
        uint32 read( RegNum index) const;
        void write( RegNum index, uint32 data);
        void reset( RegNum index); // clears register to 0 value          
        void reset( RegNum index)
        void dump( char * file);
        void resetAll() { for(int i = 0; i < MAX_REG; i++)    array[i] = 0;   }
};

class MIPS {

    friend class FuncInstr;
        RF* rf;
        uint32 PC;
        FuncMemory* mem;
        

    public:
        MIPS();
        ~MIPS();
        void load( FuncInstr& instr);
        void store( const FuncInstr& instr);
        void read_src( FuncInstr& instr);
        void ld_st( FuncInstr& instr);
        void wb( FuncInstr& instr);
        void run( const string&, uint32 instr_to_run);
        uint32 fetch() const { return mem->read( PC); }
        void updatePC( const FuncInstr& instr) { PC = instr->new_PC; }
        uint32 read(RegNum index);
        
         
   



};

 #endif // FUNC_SIM_H
