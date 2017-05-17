#ifndef H_FUNC_IN_STR
#define H_FUNC_IN_STR

//Generic C++
#include <sstream>
#include <string>

//Generis C

//uArch sim modules
#include <types.h>


class FuncInstr
{
    private:

        //define formats of instructions

        enum FORMAT
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J
        } format;

        //define types of instructions

        enum TYPE
        {
            ADD_OR_SUB,
            SHIFT,
            COND_BRANCH,
            UNCOND_BRANCH
        } type;

        // bit fields of instruction

        union
        {
            uint32 raw;            //variable to write bytes of instruction

            struct               // R-format instruction
            {
                unsigned funct:6;
		        unsigned shamt:5;
		        unsigned rd:5;
		        unsigned rt:5;
		        unsigned rs:5;
		        unsigned opcode:6;
            } asR;

            struct               // I-format instruction
            {
                signed imm:16;
                unsigned rt:5;
		        unsigned rs:5;
		        unsigned opcode:6;
            } asI;

            struct               // J-format instruction
            {
                unsigned offset:26;
		        unsigned opcode:6;
            } asJ;

        } bytes;

        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            FuncInstr::FORMAT format;
            FuncInstr::TYPE type;
        };

        static const int NUM_OF_COMMANDS = 12;
        static const int NUM_OF_REGISTERS = 32;

        // MIPS ISA table

        static const ISAEntry isaTable[NUM_OF_COMMANDS];

        //MIPS register file

        static const std::string RegFile[NUM_OF_REGISTERS];

        //---------------------------------------------------------------------------------------------
        // Internal methods to parse instruction
        //---------------------------------------------------------------------------------------------

        void initFormat( uint32 bytes);
        void initR( uint32 bytes);
        void initI( uint32 bytes);
        void initJ( uint32 bytes);
        void parseR( uint32 bytes);
        void parseI( uint32 bytes);
        void parseJ( uint32 bytes);

        //---------------------------------------------------------------------------------------------
        // Internal variables to save parametres of instruction
        //---------------------------------------------------------------------------------------------

        std::string OUT;

    public:
        FuncInstr( uint32 bytes);
        std::string Dump() const;
};

#endif

