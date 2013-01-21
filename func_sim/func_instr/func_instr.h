/**
 * func_instr.h 
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

enum NameInstr
{
    ADDI = 0x8,
    ADDIU = 0x9,
    BEQ = 0x4,
    BNE = 0x5,
    JUMP = 0x2,
    ADD = 0x20,
    ADDU = 0x21,
    SUB = 0x22,
    SUBU = 0x23,
    SLL = 0x0,
    SRL = 0x2,
    JR = 0x8,
};

const uint8 MAX_NUM_OF_INSTR = 20;
const uint8 MAX_NUM_OF_REG = 32;

enum TypeInstr
{
    R,
    I,
    J,
};

enum RegNum
{
    ZERO = 0,
    AT = 1,
    T0 = 8,
    T1 = 9,
    T2 = 10,
    T3 = 11,
    T4 = 12,
    T5 = 13,
    T6 = 14,
    T7 = 15,
    T8 = 24,
    T9 = 25
};

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

struct InstrInfo
{
    TypeInstr type;
    string name;
    uint32 opcode;
    uint32 funct;
};

const InstrInfo ISA[ MAX_NUM_OF_INSTR] = 
{
                { I, "addi", ADDI, 0x0 },
                { I, "addiu", ADDIU, 0x0 },
                { I, "beq", BEQ, 0x0 },
                { I, "bne", BNE, 0x0 },
                { J, "j", JUMP, 0x0 },
                { R, "add", 0x0, ADD },
                { R, "addu", 0x0, ADDU },
                { R, "sub", 0x0, SUB },
                { R, "subu", 0x0, SUBU },
                { R, "sll", 0x0, SLL },
                { R, "srl", 0x0, SRL },
                { R, "jr", 0x0, JR }
};//stores list of all possible instructions in our program

const string REGNAME[ MAX_NUM_OF_REG] = { "$zero", "$at",
    "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", 
    "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
    "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9",
    "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" };


class FuncInstr
{
    Converter convert; //stores bytes and makes conversion
    TypeInstr type;
    string name;
    string source;
    string target;
    string dest;
    uint32 shamt: 5;
    uint32 immed: 16;
    uint32 jump: 26;
public:
    FuncInstr( uint32 bytes);
    string Dump( string indent = " ") const;
    int setType();
    int setName();
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

