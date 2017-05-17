#include <types.h>
#include <elf_parser.h>

#include <string>


#define INSTR_NUM 12
class FuncInstr
{
    enum FormatType
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    } format;

    enum Type
    {
        ADD,
        SHT,
        COB,
        UCJ
    } type;
    
    union
    {
        // I type
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;

        // R type
        struct
        {
            unsigned func:6;
            unsigned shamt:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;

        } asR;

        // J type
        struct
        {
            unsigned address:26;
            unsigned opcode:6;
        } asJ;

        uint32 raw;
    } bytes;

    struct ISAEntry
    {
        const char* name;
        uint8 opcode;
        uint8 func;

        FuncInstr::FormatType format;
        FuncInstr::Type type;
    };

    static const ISAEntry isaTable[];
    static const std::string regTable[];

    void parseR( uint32 bytes);
    void parseI( uint32 bytes);
    void parseJ( uint32 bytes);

    void initFormat( uint32 bytes);
    std::string str; // output

	public : 
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const
        {
            return str + indent;
        }
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);
