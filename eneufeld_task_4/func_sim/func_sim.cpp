#include <iostream>

#include <stdlib.h>

#include <types.h>
#include "reg_file/reg_file.h"
#include "func_memory/func_memory.h"
#include "elf_parser/elf_parser.h"
#include "instructions.h"

typedef unsigned int PC;
struct IF_ID
{
    PC nextPC;
    uint32 instr;
};
struct ID_EX
{
    PC nextPC;
    uint32 instr;
    uint64 rs_data;
    uint64 rt_data;
    unsigned int rd_reg;
    unsigned int rt_reg;
    uint8 shamt;
    sint16 immediate;
    uint32 addres;
};
struct EX_MEM
{
    PC nextPC;
    uint32 instr;
    sint32 exe_result;
    bool brach_taken;
    unsigned int rd_reg;
    uint32 mem_addres;
};
struct MEM_WB
{
    uint32 instr;
    PC nextPC;
    sint32 data;
    unsigned int rd_reg;
};

struct IF_ID instruction_fetch( FuncMemory &memory, PC currentPC);
struct ID_EX instr_decode_reg_fetch( RegFile &registers, struct IF_ID);
struct EX_MEM exe_addr_calc( struct ID_EX);
struct MEM_WB mem_access( FuncMemory &memory, struct EX_MEM);
PC write_back( RegFile &registers, struct MEM_WB);

bool dump;

int main( int argc, char *argv[])
{
    if( argc > 3)
    {
        std::cout << "use :" << argv[0] << "'elf_file_name' [-D], -D enabled dump" << endl;
        exit( EXIT_FAILURE);
    }
    FuncMemory memory( argv[1]);
    dump = false;
    if( argc == 3)
    {
        if( argv[ 2][ 0] == '-' && argv[ 2][ 1] == 'D')
        {
            dump = true;
        }
        else
        {
            std::cout << "Unknown argument '" << argv[2] << "', ignore" << std::endl;
        }
    }
    //std::cout << memory.dump();

    RegFile registers;
    //std::cout << registers.dump();
    PC currentPC = memory.startPC(); 

    unsigned long long cycles_count = 0;
    while( currentPC != 0)
    {
        if( dump)
        {
            std::cout << "Cycle #" << cycles_count << std::endl;
        }
        currentPC = write_back( registers, 
                    mem_access( memory,
                    exe_addr_calc( 
                    instr_decode_reg_fetch( registers,
                    instruction_fetch( memory, currentPC)))));
        cycles_count++;
    }
    std::cout << "Finish!" << std::endl
              << " Results: v0 = " << registers.read( 2) << std::endl
              << "          v1 = " << registers.read( 3) << std::endl
              << "  it worked " << cycles_count << " cycles" << std::endl;
    //std::cout << registers.dump();
}


struct IF_ID instruction_fetch( FuncMemory &memory, PC currentPC) 
{
    struct IF_ID result = {0};
    result.nextPC = currentPC + 4;
    result.instr = memory.read( currentPC, 4);
    if (dump)
    {
        std::cout << "instruction fetch dump:" << std::endl 
                  << "  currentPC = 0x" << std::hex << currentPC << std::endl
                  << "  instr = 0x"  << result.instr << std::dec <<  std::endl;
    }
    return result;
};

struct ID_EX instr_decode_reg_fetch( RegFile &registers, struct IF_ID if_id)
{
    struct ID_EX result = {0};
    result.nextPC = if_id.nextPC;
    result.instr = if_id.instr;
    //decode comand
    unsigned int rs_reg = ( if_id.instr >> 21) % ( 1 << 5);
    unsigned int rt_reg = ( if_id.instr >> 16) % ( 1 << 5);
    unsigned int rd_reg = ( if_id.instr >> 11) % ( 1 << 5);
    unsigned int shamt = ( if_id.instr >> 6) % ( 1 << 5);
    int immediate = ( if_id.instr >> 0) % ( 1 << 16);
    unsigned int addres = ( if_id.instr >> 0) % ( 1 << 26);
    
    result.rs_data = registers.read( rs_reg);
    result.rt_data = registers.read( rt_reg);
    result.rd_reg = rd_reg;
    result.rt_reg = rt_reg;
    result.shamt = shamt;
    result.immediate = immediate;
    result.addres = addres;

    if (dump)
    {
        std::cout << "instr_decode_reg_fetch dump:" << std::hex << std::endl
                  << "  instr = 0x" << result.instr << std::endl
                  << "  rs_reg: " << registers.dump( rs_reg) << std::endl
                  << "  rt_reg: " << registers.dump( rt_reg) << std::endl
                  << "  rd_reg: " << registers.dump( rd_reg) << std::endl
                  << "  shamt = 0x" << shamt << std::endl
                  << "  immediate = 0x" << result.immediate << std::endl
                  << "  addres = 0x" << result.addres << std::endl 
                  << std::dec;
    }
                  
    return result;
}

struct EX_MEM exe_addr_calc( struct ID_EX id_ex)
{
    struct EX_MEM result = {0};
    unsigned int opcode = ( id_ex.instr >> 26) % ( 1 << 6);

    string op_name;
    if( opcode != R_TYPE)
    {
        if( opcode == ADDI || opcode == ADDIU)
        {
            op_name = "addi || addiu";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.rs_data + id_ex.immediate;
            result.brach_taken = false;
            result.rd_reg = id_ex.rt_reg;
            result.mem_addres = 0;
        }
        else if( opcode == BEQ || opcode == BNE)
        {
            op_name = "beq || bne";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.nextPC + 4*id_ex.immediate;
            if( id_ex.rs_data == id_ex.rt_data)
            {
                result.brach_taken = !(true ^ ( opcode == BEQ));
            }
            else
            {
                result.brach_taken = !(true ^ ( opcode == BNE));
            }
            result.rd_reg = 0;
            result.mem_addres = 0;
        }
        else if( opcode == LW || opcode == SW)
        {
            op_name = "lw || sw";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.rt_data;
            result.brach_taken = false;
            if( opcode == SW)
            {
                result.rd_reg = 0;
            }
            else
            {
                result.rd_reg = id_ex.rt_reg;
            }
            result.mem_addres = id_ex.rs_data + id_ex.immediate;
        }
        else if( opcode == LUI)
        {
            op_name = "lui";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.immediate << 16;
            result.brach_taken = false;
            result.rd_reg = id_ex.rt_reg;
            result.mem_addres = 0;
        }
        else if( opcode == J)
        {
            op_name = "j";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = ((id_ex.nextPC-4) & 0xf0000000) | (4 * id_ex.addres);
            result.brach_taken = true;
            result.rd_reg = 0;
            result.mem_addres = 0;
        }
    }
    else
    {
        unsigned int func = id_ex.instr % (1 << 6);
        if( func == ADD || func == ADDU)
        {
            op_name = "add || addu";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.rt_data + id_ex.rs_data;
            result.brach_taken = false;
            result.rd_reg = id_ex.rd_reg;
            result.mem_addres = 0;
        }
        else if( func == SUB || func == SUBU)
        {
            op_name = "sub || subu";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.rs_data - id_ex.rt_data;
            result.brach_taken = false;
            result.rd_reg = id_ex.rd_reg;
            result.mem_addres = 0;
        }
        else if( func == SLL)
        {
            op_name = "sll";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.rt_data << id_ex.shamt;
            result.brach_taken = false;
            result.rd_reg = id_ex.rd_reg;
            result.mem_addres = 0;
        }
        else if( func == SRL)
        {
            op_name = "srl";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.rt_data >> id_ex.shamt;
            result.brach_taken = false;
            result.rd_reg = id_ex.rd_reg;
            result.mem_addres = 0;
        }
        else if( func == JR)
        {
            op_name = "jr";
            result.nextPC = id_ex.nextPC;
            result.instr = id_ex.instr;
            result.exe_result = id_ex.rs_data;
            result.brach_taken = true;
            result.rd_reg = 0;
            result.mem_addres = 0;
        }
    }
    if (dump)
    {
        std::cout << "exe_addr_calc dump:" << std::hex << std::endl
                  << "  instr = 0x" << result.instr << "->" << op_name << std::endl
                  << "  exe_result = 0x" << result.exe_result << std::endl
                  << "  brach_taken = " << std::boolalpha << result.brach_taken << std::endl
                                        << std::noboolalpha
                  << "  rd_reg number: 0x" << result.rd_reg << std::endl
                  << "  mem_addres = 0x" << result.mem_addres << std::endl 
                  << std::dec;
    }
                  
    return result;
}
struct MEM_WB mem_access( FuncMemory &memory, struct EX_MEM ex_mem)
{
    struct MEM_WB result = {0};
    result.instr = ex_mem.instr;
    if( ex_mem.brach_taken == true)
    {
        result.nextPC = ex_mem.exe_result;
    }
    else
    {
        result.nextPC = ex_mem.nextPC;
    }
    if( ex_mem.mem_addres != 0)
    {
        if( ex_mem.rd_reg == 0)
        {
            memory.write( ex_mem.exe_result, ex_mem.mem_addres);
        }
        else
        {
            result.data = memory.read( ex_mem.mem_addres);
        }
    }
    else
    {
        result.data = ex_mem.exe_result;
    }
    result.rd_reg = ex_mem.rd_reg;
    if( dump)
    {
        std::cout << "mem_access dump:" << std::hex << std::endl
                  << "  instr = 0x" << result.instr << std::endl
                  << "  data = 0x" << result.data << std::endl
                  << "  rd_reg number: 0x" << result.rd_reg << std::endl 
                  << std::dec;
    }

    return result;
}
       
PC write_back( RegFile &registers, struct MEM_WB mem_wb)
{
    registers.write(mem_wb.data, mem_wb.rd_reg);
    if( dump)
    {
        std::cout << "write_back dump:" << std::hex << std::endl
                  << "  instr = 0x" << mem_wb.instr << std::endl
                  << "  nextPC = 0x" << mem_wb.nextPC << std::endl
                  << "  rd_reg : " << registers.dump( mem_wb.rd_reg) << std::endl
                  << std::dec;
    }

    if( mem_wb.rd_reg == 2)
    {
        return 0;
    }
    return mem_wb.nextPC;
}


