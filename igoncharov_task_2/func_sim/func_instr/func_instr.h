#ifndef FUNC_INSTR_H_INCLUDED
#define FUNC_INSTR_H_INCLUDED

// Generic C++
#include <iostream>
#include <cstring>
#include <cassert>

// uArchSim modules
#include <elf_parser.h>
#include <types.h>
#include <stdlib.h>
#include <func_memory.h>
#include <errno.h>


class FuncInstr
{
    union
    {
        struct
        {
            unsigned imm:   16;
            unsigned t:     5;
            unsigned s:     5;
            unsigned opcode:6;
        } asI;
        struct
        {
            unsigned funct: 6;
            unsigned shamt: 5;
            unsigned d:     5;
            unsigned t:     5;
            unsigned s:     5;
            unsigned opcode:6;
        } asR;
        struct
        {
            unsigned addres:26;
            unsigned opcode:6
        } asJ;
        uint32 raw;
    } bytes;

    public:

    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    }format;

    enum Type
    {
        ADD,
        ADDU,
        SUB,
        SUBU,
        ADDI,
        ADDIU,
        SLL,
        SRL,
        BEQ,
        BNE,
        J,
        JR,
        LAST
    }type;

    struct Registers
    {
        const char* name

        uint8 address;
    };
    static const Registers[ 32] regTable;

    struct ISAEntry
    {
        const char* name;

        uint8 opcode;
        uint8 funct;

        FuncInstr::FormatType format;
        FuncInstr::Type type;
    };
    static const ISAEntry[ LAST] isaTable;

        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
};

const FuncInstr::ISAEntry[ LAST] FuncInstr::isaTable =
{
    // name   opcode    func      format              type
    { "add ",   0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD},
    { "addu ",  0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU},
    { "sub ",   0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB},
    { "subu ",  0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU},
    { "addi ",  0x8,     0x0, FuncInstr::FORMAT_I, FuncInstr::ADDI},
    { "addiu ", 0x9,     0x0, FuncInstr::FORMAT_I, FuncInstr::ADDIU},
    { "sll ",   0x0,     0x0, FuncInstr::FORMAT_R, FuncInstr::SLL},
    { "srl ",   0x0,     0x2, FuncInstr::FORMAT_R, FuncInstr::SRL},
    { "beq ",   0x4,     0x0, FuncInstr::FORMAT_I, FuncInstr::BEQ},
    { "bne ",   0x5,     0x0, FuncInstr::FORMAT_I, FuncInstr::BNE},
    { "j ",     0x2,     0x0, FuncInstr::FORMAT_J, FuncInstr::J},
    { "jr ",    0x0,     0x8, FuncInstr::FORMAT_R, FuncInstr::JR},
};

const FuncInstr::regTable[ 32] FuncInstr::regTable =
{
    // name      address
    {  "$zero",   0x0},
    {  "$at",     0x1},
    {  "$v0",     0x2},
    {  "$v1",     0x3},
    {  "$a0",     0x4},
    {  "$a1",     0x5},
    {  "$a2",     0x6},
    {  "$a3",     0x7},
    {  "$t0",     0x8},
    {  "$t1",     0x9},
    {  "$t2",     0xA},
    {  "$t3",     0xB},
    {  "$t4",     0xC},
    {  "$t5",     0xD},
    {  "$t6",     0xE},
    {  "$t7",     0xF},
    {  "$s0",     0x10},
    {  "$s1",     0x11},
    {  "$s2",     0x12},
    {  "$s3",     0x13},
    {  "$s4",     0x14},
    {  "$s5",     0x15},
    {  "$s6",     0x16},
    {  "$s7",     0x17},
    {  "$t8",     0x18},
    {  "$t9",     0x19},
    {  "$k0",     0x1A},
    {  "$k1",     0x1B},
    {  "$gp",     0x1C},
    {  "$sp",     0x1D},
    {  "$fp",     0x1E},
    {  "$ra",     0x1F},
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif // FUNC_INSTR_H_INCLUDED
