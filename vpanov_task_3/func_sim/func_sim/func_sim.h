

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include <types.h>
#include <func_memory.h>
#include <func_instr.h>


enum RegNum {
    REG_ZERO,
    REG_AT,
    REG_V0, REG_V1,
    REG_A0, REG_A1, REG_A2, REG_A3,
    REG_T0, REG_T1, REG_T2, REG_T3, REG_T4, REG_T5, REG_T6, REG_T7,
    REG_S0, REG_S1, REG_S2, REG_S3, REG_S4, REG_S5, REG_S6, REG_S7,
    REG_T8, REG_T9,
    REG_K0, REG_K1,
    REG_GP,
    REG_SP,
    REG_FP,
    REG_RA,
    MAX_REG = 32
};

class RF {
    uint32 array[MAX_REG];
public:
    uint32 read(RegNum index);
    void write(RegNum index, uint32 data);
    void reset(RegNum index);
    std::string dump(std::string indent = "") const;
};

typedef FuncMemory Memory;

class MIPS {
    RF *rf;
    uint32 PC;
    Memory *mem;

    void read_src(FuncInstr&);
    void wb(FuncInstr&);
    void load(FuncInstr&);
    void store(const FuncInstr&);
    void ld_st(FuncInstr& instr);

public:
    MIPS();
    ~MIPS();
    void run(const string&, uint instr_to_run);
    uint32 fetch() { return (uint32)mem->read(PC); }
    void updatePC(const FuncInstr&);
    string dump(string indent = "") const;
};


#endif //FUNC_SIM_H