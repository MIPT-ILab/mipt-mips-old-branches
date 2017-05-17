#ifndef FUNCINSTR
#define FUNCINSTR
#include <types.h>
#include <instructions.h>

class FuncInstr
{
    private:
        int type;
        int instruction;
        int source_reg;
        int target_reg;
        int dest_reg;
        int shamt;
        int constant;
        int address;
    
        int GetType( uint32 bytes);
        int GetRInstr( uint32 bytes);
        int GetIInstr( uint32 bytes);
        int GetJInstr( uint32 bytes);
    public:
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNCINSTR
