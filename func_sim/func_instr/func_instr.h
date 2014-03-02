/**
 *
 *
 *
 *
 */

#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

#include "../../common/types.h"
//Generic C++
#include <iostream>


//Generic C
#include "stdio.h"
#include "assert.h"

class FuncInstr
{
    private:
        int initFormat( uint32 bytes);
        int parseR( );
        int parseI( );
        int parseJ( );
        std::string textAss;
    public:
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
        

        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
        } format; 
        enum Type
        {
            ADD,
            ADDU,
            SUB,
            SHIFT,
            BRANCH,
            END_OF_INSTRUCTIONS_SET
        }type;

        union
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
                unsigned shamt:5;
                unsigned d:5;
                unsigned t:5;
                unsigned s:5;
                unsigned opcode:6;
            } asR;
            struct
            {
                unsigned address:26;
                unsigned opcode:6;                
            } asJ;
            uint32 raw;
        }bytes;

        struct ISAEntry
        {
            const char* name;
            uint8 opcode;
            uint8 func;
            FuncInstr::Format format;
            FuncInstr::Type type;
        };
        static const ISAEntry isaTable[];
        static const char* registersTable[];
};

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr);

#endif // FUNC_INSTR__FUNC_INSTR_H

