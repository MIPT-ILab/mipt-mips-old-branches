#ifndef FUNC_SIM_H_INCLUDED
#define FUNC_SIM_H_INCLUDED

#include "func_memory.h"
#include "func_instr.h"

enum RegNum {
      ZERO = 0, AT,  
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
      MAX_REG = 32
};

class RF {
      uint32 array[MAX_REG];

    public:

      uint32 read( RegNum index) const;
      void write( RegNum index, uint32 data);
      void reset( RegNum index); // clears register to 0 value
      bool check( RegNum index) const; // check if register exist
};

class MIPS 
{
        // storages of internal state
        RF* rf;
        uint32 PC;
        FuncMemory* mem;

        uint32 fetch() const { return mem->read( PC); }
        void updatePC( const FuncInstr& instr) { PC = instr.new_PC; }
        void read_src( FuncInstr& instr);
        void ld_st( FuncInstr& instr);
        void load( FuncInstr& instr);
        void store( /*const*/ FuncInstr& instr);
        void write_back( FuncInstr& instruction);

    public:

        MIPS();
        ~MIPS();
        void run( const string&, uint32 instr_to_run);
};

#endif // FUNC_SIM_H_INCLUDED
