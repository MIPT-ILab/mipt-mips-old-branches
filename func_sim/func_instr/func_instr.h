/**
 * func_instr.h - Header of the instruction decoder 
 * author Alexander Kravtcov
 */

// protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <string>

// uArchSim modules
#include <types.h>

class FuncInstr
{
    // declaration some private types and constants
    enum TypeFormat
    {
        R_TYPE,
        I_TYPE,
        J_TYPE
    };
    
    union Convert
    {
        uint32 bytes; // original convertible data
        
        struct // see on instruction as R-type 
        {
            unsigned funct:6;
            unsigned shamt:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asR;
    
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;
    
        struct
        {
            unsigned imm:26;
            unsigned opcode:6;
        } asJ;
    };
    
    enum ArgOrderPrint
    {
        DST, // will be printed: $d, $s, $t
        TSI, // will be printed: $s, $t, IMM
        DTI,
        STI,
        TS,
        I,
        S,
        T,
        TIS, // will be printed: $t, IMM($t)
        TI,
        NOT // no argement will be printed
    };
    
    enum InstrNames
    {
       ADD, ADDU, SUB, SUBU, ADDI, ADDIU, SLL, SRL, BEQ, BNE, J, JR, LW, SW, LUI
    };
    
    struct InstrInfo
    {
        InstrNames name;
        std::string name_str; // printed name of instruction (e.g. "add", "sll")
        unsigned opcode;
        unsigned funct;
        TypeFormat type;
    
        // the presense or absense arguments
        bool is_reg_s;
        bool is_reg_t;
        bool is_reg_d;
        bool is_imm;
    
        ArgOrderPrint arg_order;
    };
    
    // for marking unused filds in list of instructions
    static const unsigned UNUSED = -1;

    static const int SIZE_OF_ISA = 15;
    static const int SIZE_OF_REGISTERS = 32;
    
    static const InstrInfo ISA[ SIZE_OF_ISA]; // define in .cpp file
    
    enum Registers
    {
        ZERO, AT,
        V0, V1,
        A0, A1, A2, A3,
        T0, T1, T2, T3, T4, T5, T6, T7,
        S0, S1, S2, S3, S4, S5, S6, S7,
        T8, T9,
        K0, K1,
        GP, SP, FP, RA
    };
    
    struct RegName
    {
        enum Registers reg;
        std::string name_str;
    };
    
    static const RegName RegNames[ SIZE_OF_REGISTERS]; // define in .cpp file
    // end of declaration some private types and constants



    InstrInfo instr;

    unsigned imm;  // contain constant, which can be in instruction
    RegName reg_s; //
    RegName reg_t; // registers
    RegName reg_d; //

    int num_of_arguments; // which are after name of instruction

    // contain command with argements for print ("add $s0, %t0, %t1")
    std::string dump_str; 

    Convert convertible;

    void formDumpStr();

    // define what is the instruction via choice from the list of ISA
    InstrInfo selection() const;

    // fills registers and imm fields and checks pseudo-instructions
    void parseInstr();

    // for unusing default constructor
    FuncInstr(){}
public:
        FuncInstr( uint32 bytes);

        std::string dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H

