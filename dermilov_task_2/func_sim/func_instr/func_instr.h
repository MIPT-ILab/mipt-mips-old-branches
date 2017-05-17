#ifndef FUNC_INSTR_H_INCLUDED
#define FUNC_INSTR_H_INCLUDED

#include <types.h>
class FuncInstr
{
    // it's union for easier parsing
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
    } bytes;

    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    } format;

    enum Type
    {
        SLL = 0x0, 
        SRL = 0x2, 
        BEQ = 0x4, 
        BNE = 0x5, 
        ADDI = 0x8, 
        ADDIU = 0x9,
        ADD = 0x20, 
        ADDU = 0x21, 
        SUB = 0x22, 
        SUBU = 0x23,
        J = 0x2, 
        JR = 0x8
    } type;

    enum Registers
    {
        ZERO = 0, 
        AT,  
        T0 = 8, 
        T1, 
        T2, 
        T3, 
        T4, 
        T5, 
        T6, 
        T7,
        T8 = 24, 
        T9
    } dest, source, target;

    uint32 immediate;
    uint64 address;


    struct ISAEntry
    {
        const char* name;

        uint8 opcode;
        uint8 func;

        FuncInstr::Format format;
        FuncInstr::Type type;
    };
    static const ISAEntry isaTable[];
    uint32 tab_isa_size;

    struct REGEntry
    {
        const char* name;

        FuncInstr::Registers reg;
    };
    static const REGEntry regTable[];// this table assosiates register's number and name 
    uint32 tab_reg_size;

    FuncInstr();

public:
    FuncInstr( uint64 bytes);
//    ~FuncInstr();
    void initFormat( uint32 bytes);
    std::string Dump( std::string indent = " ") const;
    void parseR( uint32 bytes);
    void parseI( uint32 bytes);
    void parseJ( uint32 bytes);
    void put_reg( int reg, enum Registers &field);
    bool check_reg( enum Registers reg) const;
    bool check_typeR( enum Type type) const;
    bool check_typeI( enum Type type) const;
    bool check_typeJ( enum Type type) const;
    int get_reg_num( enum Registers reg) const;
    int get_isa_num() const;
    void dumpR( std::string &str) const;
    void dumpI( std::string &str) const;
    void dumpJ( std::string &str) const;
};


std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif // FUNC_INSTR_H_INCLUDED
