/*
 * func_instr.h
 *
 *  Created on: Feb 7, 2014
 *      Author: thar0l
 */

#ifndef FUNC_INSTR_FUNC_INSTR_H_
#define FUNC_INSTR_FUNC_INSTR_H_

// Generic C++


// uArchSim modules
#include <types.h>
#include <elf_parser.h>

#define INSTRUCTIONS_COUNT  12
#define PSEUDO_INSTRUCTIONS_COUNT 3
#define REGISTERS_COUNT 32

class FuncInstr
{

    enum FormatType
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    };

    enum Type
    {
        MATH,
        SHIFT,
        C_BRANCH,
        UC_BRANCH
    } ;

    union
    {
      uint32 raw;
      struct
      {
          signed imm:16;
          unsigned rt:5;
          unsigned rs:5;
          unsigned opcode:6;
      } asI;
      struct
      {
          unsigned offset:26;
          unsigned opcode:6;
      } asJ;
      struct
      {
          unsigned funct:6;
          unsigned shamt:5;
          unsigned rd:5;
          unsigned rt:5;
          unsigned rs:5;
          unsigned opcode:6;
      } asR;
    } bytes;

    FormatType format;

    struct ISAEntry
    {
        const char* name;

        uint8 opcode;
        uint8 func;

        FuncInstr::FormatType format;
        FuncInstr::Type type;
    };

    static const ISAEntry    isaTable[];
    static const std::string regTable[];
    static const std::string piTable[];
    std::string dump_str;
    public:

        FuncInstr( uint32 bytes);
        void initFormat( uint32 bytes);
        void init();
        void parseR( uint32 bytes);
        void parseI( uint32 bytes);
        void parseJ( uint32 bytes);
        std::string Dump( std::string indent = " ") const
        {
            return indent+dump_str;
        }

};





#endif /* FUNC_INSTR_FUNC_INSTR_H_ */
