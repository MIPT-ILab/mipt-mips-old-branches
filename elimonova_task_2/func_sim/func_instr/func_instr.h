/**
 * func_instr.h - Header of FuncInstr class
 * @author Elena Limonova <elena.e.limonova@gmail.com>
 * Copyrignt 2012 uArchSim iLab project
 */

#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

#include <string>

#include <types.h>

class FuncInstr
{
        enum FormatType  { TYPE_J, TYPE_I, TYPE_R};
        enum Instruction { INSTR_ADD, INSTR_ADDU, INSTR_SUB, INSTR_SUBU, INSTR_ADDI, INSTR_ADDIU, INSTR_SLL, INSTR_SRL, INSTR_BEQ, INSTR_BNE, 
                           INSTR_J, INSTR_JR, INSTR_MOVE, INSTR_CLEAR, INSTR_NOP};
        enum PrintType   { PRINT_DST, PRINT_TSC, PRINT_DTC, PRINT_STC, PRINT_C, PRINT_S, PRINT_TS, PRINT_T, PRINT_NONE}; //Output type
        enum Register    { REG_ZERO, REG_AT, REG_V0, REG_V1, REG_A0, REG_A1, REG_A2, REG_A3, REG_T0, REG_T1, REG_T2, REG_T3, REG_T4, REG_T5, 
                           REG_T6, REG_T7, REG_S0, REG_S1, REG_S2, REG_S3, REG_S4, REG_S5, REG_S6, REG_S7, REG_T8, REG_T9, REG_K0, REG_K1, 
                           REG_GP, REG_SP, REG_FP, REG_RA};

        FormatType  type; 
        PrintType   pType;
        std::string name; 
        Instruction instr;
        Register    source;
        Register    target;
        Register    dest;
        uint32      shamt; //constant value for shifts
        uint32      iConstVal; //immediate constant value for ALU operations
        uint32      address; //address for jumps and procedure calls
        uint32      opcode;
        uint32      funct;      
  
        void        convertInstr( FormatType type, uint32 instr);
        FormatType  getType( uint32 opcode);
        uint32      getOpcode( uint32 bytes);
        uint32      getFunct( uint32 bytes);
 
        struct InstrInfo
        {
                std::string name;
                Instruction instr;
                PrintType   pType;
                FormatType  type;
                uint32      opcode;
                uint32      funct;
        };
        static const char * myRegNames[];
        union Convert //decode instruction for each type
        {
            struct
            {
                uint32 funct : 6;
                uint32 shamt : 5;
                uint32 d : 5;
                uint32 t : 5;
                uint32 s : 5;
                uint32 opcode : 6;
            } asR;
            struct
            {
                uint32 iConst : 16;
                uint32 t : 5;
                uint32 s : 5;
                uint32 opcode : 6;
            } asI;
            struct 
            {
                uint32 addr : 26;
                uint32 opcode : 6;
            } asJ;
        };
    public:
        static InstrInfo ISA[];
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif
