/*
 * Header for register file, which can keep state of registers,
 * allow extern fuctions to read and write this state
 * author Alexander Kravtcov
 */

#ifndef REG_FILE_HEADER
#define REG_FILE_HEADER

// uArchSim module
#include <types.h>

class RegFile
{
  public:
    enum Registers
    {
        ZERO, AT,
        V0, V1,
        A0, A1, A2, A3,
        T0, T1, T2, T3, T4, T5, T6, T7,
        S0, S1, S2, S3, S4, S5, S6, S7,
        T8, T9,
        K0, K1,
        GP, SP, FP, RA,
        SIZE_OF_REGISTERS // can not be as a parameter for read and write
    };
    
    RegFile();
    
    uint64 read( RegFile::Registers reg);
    void write( uint64 value, RegFile::Registers reg);

  private:
    uint64 reg_file[ SIZE_OF_REGISTERS];
};

#endif

