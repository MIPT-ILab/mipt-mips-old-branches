#ifndef FUNC_SIM_H
#define FUNC_SIM_H


#include <types.h>
#include <func_memory.h>

#include <func_instr.h>

class FuncInstr;

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

class RF {
        uint32 array[MAX_REG];
    public:
        RF();

        uint32 read( RegNum index) const { return array[index];}
        void write( RegNum index, uint32 data) { if(index!=$zero) array[index] = data;}
        void reset( RegNum index) { array[index] = 0;}
        void dump( char * file);
        void resetAll() { for(int i = 0; i < MAX_REG; i++)    array[i] = 0;   }
};

class MIPS {

    friend class FuncInstr;
        // storages of internal state
        RF* rf;
        uint32 PC;
        FuncMemory* mem;
        void load( FuncInstr& instr);
        void store( const FuncInstr& instr);

    public:
        MIPS();

        void ld_st( FuncInstr& instr);

        void run( const string& , uint32 instr_to_run);
        uint32 fetch() const;/* {
            return mem->read( PC, 4);
        }*/
        void updatePC( const FuncInstr& instr);
        uint32 read( RegNum index);
        void read_src( FuncInstr& instr);
        void wb( FuncInstr& instr);

};

#endif // FUNC_SIM_H
