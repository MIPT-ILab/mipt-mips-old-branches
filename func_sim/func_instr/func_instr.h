/**
 * func_instr.h 
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <iostream>
#include <string>

// uArchSim modules
#include <types.h>

using namespace std;

class FuncInstr
{
    static const uint32 MAX_NUM_OF_INSTR = 20; //max quantity of the instructions that we can parse.
    static const uint32 MAX_NUM_OF_REG = 32; //max quantity of MIPS registers.
    static const uint32 NULL_VAL = 0x0; 

    //Opcode/funct of instructions.
    enum NameInstr
    {
        ADDI  = 0x8,
        ADDIU = 0x9,
        BEQ   = 0x4,
        BNE   = 0x5,
        JUMP  = 0x2,
        ADD   = 0x20,
        ADDU  = 0x21,
        SUB   = 0x22,
        SUBU  = 0x23,
        SLL   = 0x0,
        SRL   = 0x2,
        JR    = 0x8,
    };

    //possible types of instructions.
    enum TypeInstr
    {
        R_TYPE,
        I_TYPE,
        J_TYPE,
    };

    //convert number into different types of instructions.
    union Converter 
    {
        uint32 asBytes;
        struct
        {
            uint32 funct: 6;
            uint32 shamt: 5;
            uint32 dest: 5;
            uint32 target: 5;
            uint32 source: 5;
            uint32 opcode: 6;
        } asR;
        struct 
        {
            uint32 immed: 16;
            uint32 target: 5;
            uint32 source: 5;
            uint32 opcode: 6;
        } asI;
        struct 
        {
            uint32 jump: 26;
            uint32 opcode: 6;
        } asJ;
    };
    //contain information about instruction
    struct InstrInfo
    {
        TypeInstr type;
        string name;
        uint32 opcode;
        uint32 funct;
    };
    
    struct TurnPseudo
    {
        bool nop;
        bool move;
        bool clear;
    };

    static TurnPseudo pseudo_instr;
    //stores list of all possible instructions in our program
    static const InstrInfo ISA[ MAX_NUM_OF_INSTR];
    //stores names of all MIPS registers 
    static const string REGNAME[ MAX_NUM_OF_REG];
    
    Converter convert; //stores bytes and makes conversion
    TypeInstr type;
    string name;
    uint32 opcode: 6;
    uint32 funct: 6;
    string source;
    string target;
    string dest;
    uint32 shamt: 5;
    uint32 immed: 16;
    uint32 jump: 26;
    //You can't create object with this default constructor
    FuncInstr();
public:
    explicit FuncInstr( uint32 bytes);
    string Dump( string indent = " ") const;
    int setType();
    void setNameForRtype(); //set name, opcode and funct in R type instr.
    void setNameForIandJtypes(); //-|-|- in I and J type instructions
    void setSource();
    void setTarget();
    void setDest();
    void setShamt();
    void setImmed();
    void setJump();
    bool is_nop() const;
    bool is_move() const;
    bool is_clear() const;
};

ostream& operator << ( ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H

