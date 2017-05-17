#include <types.h>
#include <string>
#include <sstream>

class FuncInstr
{

    public:
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;

    private:

        enum FormatType
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J
        } format;

        enum Type
        {
            ADD,
            ADDU,
            SUB,
            SUBU,
            ADDI,
            ADDIU,
            SRL,
            BEQ,
            BNE,
            J,
            JR,
            SLL,
            ISAEND
        };

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
                unsigned func:6;
                unsigned sha:5;
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
            struct
            {
                unsigned raw:26;
                unsigned opcode:6;
            } asDefault;
            uint32 raw;
        } bytes;

        struct ISAEntry
        {
            const char* name;

            uint8 opcode;
            uint8 func;

            FuncInstr::FormatType format;
            FuncInstr::Type type;
            // ...
        };
        static const ISAEntry isaTable[];

        static const char* regTable[];

        std::ostringstream parsed;

        int parseR();
        int parseI();
        int parseJ();
        int initFormat();
};
