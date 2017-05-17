/**
 * func_instr.h
 * Header of the MIPS instruction decoder.
 * MIPT-MIPS-2014 Assignment 2.
 * Ladin Oleg.
 */

// Protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <iostream>

// uArchSim modules
#include <types.h>

/** FuncInstr class is an abstraction of MIPS instruction. */
class FuncInstr
{
    private:

        /* Name of group of instruction. */
        enum Group
        {
            GROUP_ADD_SUBTRACT,
            GROUP_SHIFTS,
            GROUP_CONDITIONAL_BRANCHES,
            GROUP_UNCONDITIONAL_JUMP,
            NO_GROUP
        };
        /* Name of format of instruction. */
        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            NO_FORMAT
        };
        /**
         * Complete description of each instruction: name of group, it's name,
         * assembler code, format, opcode, funct, bit-field mask and index
         * number for pointing respectively. Last element of massive is end
         * member - it stores next values: "NO_GROUP", 0, 0, "NO_FORMAT", 0xFF,
         * 0xFF, 0, 0xFF.
         * 'fields' is flag-and-number bit element, which consist of:
         * "FFstdh--11223344", there: FF - format (00 - R, 01 - I, 10 - J); s,
         * t, d, h - flags of using respectively register/constant fields (0 -
         * not used, 1 - used; R: s|t|d|S, I: s|t|C|-, J: A|-|-|-); 11, 22,
         * 33, 44 - number of 1st, 2nd, 3rd and 4th output position (R: 00 -
         * rs, 01 - rt, 10 - rd, 11 - sh; I: 00 - rs, 01 - rt, 10 - imm; J: 00
         * - add; R: 11|22|33|44, I: 11|22|33|--, J: 11|--|--|--). "-" - not
         * using bit, assumed to be zeroed.
         * Example: sll - 00011100|10011100 - R; using rt, rd, sh; output
         * order: rd-rt-sh-rs (last is ignored, but imply, that R generally
         * uses rs).
         */
        struct ISAEntry
        {
            Group group;
            char* name;
            char* code;
            Format format;
            uint8 opcode;
            uint8 func;
            uint16 fields;
            uint8 number;
        };

        /*
         * Output structure. 'type' - number (2), string (1) or not uses (0).
         * Numeric value stores in 'num', string - 'str'. It has 5 fields:
         * first for code and next 4 for register/constant values (arguments).
         */
        struct Dmp
        {
            uint8 type;
            char* str;
            uint32 num;
        } dmp[ 5];
        /*
         * Union is able to store every format in one place and to disassemble
         * input bytes on groups according their format.
         */
        union
        {
            struct
            {
                unsigned func : 6;
                unsigned sh : 5;
                unsigned rd : 5;
                unsigned rt : 5;
                unsigned rs : 5;
                unsigned opcode : 6;
            } asR;
            struct
            {
                unsigned imm : 16;
                unsigned rt : 5;
                unsigned rs : 5;
                unsigned opcode : 6;
            } asI;
            struct
            {
                unsigned add : 26;
                unsigned opcode : 6;
            } asJ;
            uint32 raw;
        } bytes;
        uint8 num; // Number of isa_table string and instruction identificator

        static const ISAEntry isa_table[]; // Array of instructions
        static const char* reg_table[]; // Array of register names

        /* Functions to initialize ond parse input bytes. */
        void initFormat( uint32 bytes);
        void parseR( uint32 bytes);
        void parseJ( uint32 bytes);
        void parseI( uint32 bytes);

    public:

        FuncInstr( uint32 bytes); // Constructor parse and save instruction
        std::string Dump( std::string indent = " ") const; // Prints instruction
};

/* Reload operator for more comfortable output. */
std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H
