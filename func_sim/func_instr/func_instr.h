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
        };

        // bit fields of instruction

        union
        {
            struct               // R-format instruction
            {
                unsigned opcode:6;
                unsigned rs:5;
                unsigned rt:5;
                unsigned rd:5;
                unsigned shamt:5;
                unsigned funct:6;
            } asR;

            struct               // I-format instruction
            {
                unsigned opcode:6;
                unsigned rs:5;
                unsigned rt:5;
                signed imm:16;
            } asI;

            struct               // J-format instruction
            {
                unsigned opcode:6;
                unsigned offset:26;
            } asJ;
            uint32 raw;            //variable to write bytes of instruction

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

