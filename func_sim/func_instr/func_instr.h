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
#include <reg_file.h>

class FuncInstr
{
  public: 
    enum Instructions
    {
       ADD, ADDU, SUB, SUBU, ADDI, ADDIU, SLL, SRL, BEQ, BNE, J, JR, LW, SW,
       LUI, NOP, SIZE_OF_INSTR
    };
  
  private:
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
            unsigned short imm:16; // might be used as signed
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;
    
        struct
        {
            unsigned addr:26;
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
        DS,
        I,
        S,
        T,
        TIS, // will be printed: $t, IMM($t)
        TI,
        NOT // no argement will be printed
    };

    struct InstrInfo
    {
        Instructions instr;
        std::string name_str; // contain name of instruction (e.g. "add", "sll")
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
    
    static const InstrInfo ISA[ SIZE_OF_INSTR]; // define in .cpp file
    
    struct RegName
    {
        RegFile::Registers reg;
        std::string name_str;
    };
    
    // define in .cpp file
    static const RegName RegNames[ RegFile::SIZE_OF_REGISTERS];

    // for marking unused filds in list of instructions
    static const unsigned UNUSED = -1;
    // end of declaration some private types and constants


    InstrInfo instr_info;

    uint64 imm;  // contain constant, which can be in instruction
    RegName reg_s; //
    RegName reg_t; // registers
    RegName reg_d; //

    uint64 data_src1; //
    uint64 data_src2; // data of registers for dump
    uint64 data_dst;  //
    
    int num_of_arguments; // which are after name of instruction

    // contain command with argements for print ("add $s0, %t0, %t1")
    //std::string dump_str; 

    Convert convertible;

    std::string formDumpStr() const;

    // define what is the instruction via choice from the list of ISA
    InstrInfo selection() const;

    // fills registers and imm fields and checks pseudo-instructions
    void parseInstr();

    // for unusing default constructor
    FuncInstr(){}

public:
    FuncInstr( uint32 bytes);

    void setDataOfRegs( uint64 data_src1 = 0, uint64 data_src2 = 0,
                        uint64 data_dst = 0);

    Instructions getInstr();    

    uint64 getImm();
    RegFile::Registers getRegS();
    RegFile::Registers getRegT();
    RegFile::Registers getRegD();

    std::string dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H

