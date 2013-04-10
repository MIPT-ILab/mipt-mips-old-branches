/**
 * func_instr.h - Header of the instruction decoder 
 * @author Alexander Kravtcov
 */

// protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <string>

// uArchSim modules
#include <types.h>

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

struct InstrInfo
{
    std::string name; // name of instruction ( e.g. "add", "sll")
    unsigned opcode;
    unsigned funct;
    TypeFormat type;

    // the presense or absense arguments
    bool is_reg_s;
    bool is_reg_t;
    bool is_reg_d;
    bool is_imm;

    // in most MIPS-instructions order of registers is $d, $s ,$t
    bool is_right_order_reg;
};

const unsigned UNUSED = -1; // for marking unused filds in list of instructions
const int SIZE_OF_ISA = 12;

const InstrInfo ISA[] = { { "add", 0x0, 0x20, R_TYPE, 1, 1, 1, 0, 1},
                          { "addu", 0x0, 0x21, R_TYPE, 1, 1, 1, 0, 1},
                          { "sub", 0x0, 0x22, R_TYPE, 1, 1, 1, 0, 1},
                          { "subu", 0x0, 0x23, R_TYPE, 1, 1, 1, 0, 1},
                          { "addi", 0x8, UNUSED, I_TYPE, 1, 1, 0, 1, 0},
                          { "addiu", 0x9, UNUSED, I_TYPE, 1, 1, 0, 1, 0},
                          { "sll", 0x0, 0x0, R_TYPE, 0, 1, 1, 1, 1},
                          { "srl", 0x0, 0x2, R_TYPE, 0, 1, 1, 1, 1},
                          { "beq", 0x4, UNUSED, I_TYPE, 1, 1, 0, 1, 1},
                          { "bnq", 0x5, UNUSED, I_TYPE, 1, 1, 0, 1, 1},
                          { "j", 0x2, UNUSED, J_TYPE, 0, 0, 0, 1, 1},
                          { "jr", 0x0, 0x8, R_TYPE, 1, 0, 0, 0, 1}};

const std::string RegNames[] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1",
                                 "$a2", "$a3", "$t0", "$t1", "$t2", "$t3",
                                 "$t4", "$t5", "$t6", "$t7", "$s0", "$s1",
                                 "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                                 "$t8", "$t9", "$k0", "$k1", "$gp", "$sp",
                                 "$fp", "$ra"};

class FuncInstr
{
    InstrInfo instr;

    unsigned imm;  // contain constant, which can be in instruction
    std::string reg_s; //
    std::string reg_t; // registers
    std::string reg_d; //

    int num_of_arguments; // which are after name of instruction

    Convert convertible;

    InstrInfo selection() const; // from list of ISA
    void parseInstr();

    // for unusing default constructor
    FuncInstr(){}
public:
        FuncInstr( uint32 bytes);

        std::string dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H
