/*
 * func_instr.h - instruction parser for mips
 * @author Aleksandr Kayda aleksandr.kajda@frtk.ru
 * Copyright 2014 uArchSim iLab 
 */


#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>



class FuncInstr
{

    public:
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;

    private:


        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        } format;

        enum OutputType
        {
            OUT_R_ARITHM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_J_JUMP
        } outputType;

        union 
        {
            struct
            {
                unsigned funct  :6;
                unsigned shamt  :5;
                unsigned rd     :5;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asR;
            struct
            {
                unsigned imm    :16;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asI;
            struct
            {
                unsigned imm    :26;
                unsigned opcode :6;
            } asJ;
            uint32 raw;
        } instr;
        struct ISAEntry
        {
            const char* name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OutputType outputType;
        };
        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        uint32 isaNum;
        static const char *regTable[32];
        std::string disasmStr;
                                                               
        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();
        int checkPseudo();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);


#endif //FUNC_INSTR_H

