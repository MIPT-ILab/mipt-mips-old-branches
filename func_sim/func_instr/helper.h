/**
 * helper.h - the module implementing a simple disassembler for MIPS processors.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */


// protection from multi-include
#ifndef FUNC_MEMORY__HELPER_H
#define FUNC_MEMORY__HELPER_H

#include <string>

namespace helper
{
    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    };

    enum Type
    {
        ADD,
        ADDI,
        ADDU,
        ROLL,
        BEQ,
        JUMP,
        JR,
        MOVE,
        CLR,
        NOP
    };

    union Parser
    {
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;

        struct
        {
            unsigned funct:6;
            unsigned S:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asR;

        struct
        {
            unsigned addr:26;
            unsigned opcode:6;
        } asJ;

        uint32 raw;
    };

    struct ISAEntry
    {
        char name[ 16];

        uint8 opcode;
        uint8 funct;

        Format format;
        Type type;
    };



}; // namespace helper

#endif


