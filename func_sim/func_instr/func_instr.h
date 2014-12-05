/**
 * func_instr.h - Header of disassembler
 * @author Semyon Abramov <semyon.abramov.mipt@gmail.com>
 * 2014 MIPT-MIPS iLab project
 */

// protection from multi-include
#ifndef FUNC_INSTR
#define FUNC_INSTR

//Generic C++
#include<iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

//Generic C
#include<assert.h>
#include <string.h>

//Generic uArchsim modules
#include"types.h"



using namespace std;

class FuncInstr
{

    private:
        static const int isaTable_size = 14;            // Size of ISAEntry Table
        const char* oper_name;                          // Name of MIPS operation
        const char* Reg_t;                              // Name of Target register
        const char* Reg_s;                              // Name of Source register
        const char* Reg_d;                              // Name of Destination register
        uint32 immediate;                               // Immediate value for arithmetic and logical (AL) operations
        uint32 shamt;                                   // Constant value for shifts
        uint32 jump_addr;                               // Address for jumps and procedure calls

        /* Enumeration of operations' types used in MIPS ISA */
        enum Type
        {
            ADD,      // add
            ADDU,     // add unsigned
            SUB,      // substract
            SUBU,     // substract unsigned
            ADDI,     // add immediate
            ADDIU,    // add immediate unsigned
            SLL,      // shift left logical
            SRL,      // shift right logical
            BEQ,      // branch on equal
            BNE,      // branch on not equal
            J,        // jump
            JR,       // jump register
            UNDEF     // undefined operation
        }type;

        /* Enumertion of operations' formats used in MIPS ISA */
        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            UNDEF_FORMAT
        } format;

        /* This union helps to parse bytes according to operation format */
        union
        {
            /* This struct is used to parse bytes according to I type */
            struct
            {
                unsigned imm:16;
                unsigned t:5;
                unsigned s:5;
                unsigned opcode:6;
            } asI;

            /* This struct is used to parse bytes according to R type */
            struct
            {
                unsigned funct:6;
                unsigned shamt:5;
                unsigned d:5;
                unsigned t:5;
                unsigned s:5;
                unsigned opcode:6;
            } asR;

            /* This struct is used to parse bytes according to J type */
            struct
            {
                unsigned addr:26;
                unsigned opcode:6;
            } asJ;

            uint32 raw;
        } bytes;

        /* This struct need to implement ISA Table */
        struct ISAEntry
        {
                const char* name;
                uint32 opcode;
                uint32 func;
                FuncInstr::Format format;
                FuncInstr::Type type;
                uint8 counter;

        };
        Type oper_type;

    public:
        FuncInstr( uint32 bytes);

        #if DESTRUCTOR
        ~FuncInstr( uint32 bytes);                              //there are no objects which must be deleted
        #endif
        void initFormat( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
        void parseR( uint32 bytes);
        void parseJ( uint32 bytes);
        void parseI( uint32 bytes);
        static const ISAEntry isaTable[ ];
        static const char* regTable[ ];


};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif FUNC_INSTR

