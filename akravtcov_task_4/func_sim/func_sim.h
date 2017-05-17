/*
 * func_sim.h - Header for interface of a functional simulator
 * author Alexander Kravtcov
 */

#ifndef FUNC_SIM_HEADER
#define FUNC_SIM_HEADER

#include <types.h>
#include <func_instr.h>
#include <reg_file.h>
#include <func_memory.h>

class FuncSim
{
    struct Control
    {
        FuncInstr::Instructions alu_op;
        bool reg_source1_t; // for shift-instructions
        bool reg_dest_t;
        bool reg_write;
        bool alu_src_imm;
        bool mem_read;
        bool mem_write;
        bool mem_to_reg;
    };

    // contains control signals for all instructions
    static const Control controls[ FuncInstr::SIZE_OF_INSTR];


    RegFile reg_file;
    FuncMemory func_memory;
    FuncInstr instr;

    unsigned int num_of_executed_instr;
    
    uint32 PC; // programm counter
    Control current_control;
    
    RegFile::Registers read_reg1;
    RegFile::Registers read_reg2;
    RegFile::Registers write_reg;

    uint64 imm;
    uint64 reg_read_data1;
    uint64 reg_read_data2;
    uint64 reg_write_data;

    uint64 alu_src_data1;
    uint64 alu_src_data2;
    uint64 alu_result;
    bool alu_zero; // sets if result of alu is zero

    uint32 mem_read_addr;
    uint32 mem_write_addr;
    uint64 mem_read_data;
    uint64 mem_write_data;

    // sets if some instructions write to $v0 register
    bool exit_flag;


    // we need in this multiplexer functions to choose a source 
    // for the block units ( register file, alu, memory, write back)
    RegFile::Registers muxSrcReg1();
    RegFile::Registers muxWriteReg();
    uint64 muxAluSrc();
    uint64 muxWriteDataToRegFile();

public:
    
    FuncSim( const char* executable_file_name);

    void decode();
    void readFromReg();
    void alu();
    void memReadWrite();
    void writeBack();
    void updatePC();

    bool getExitFlag();
    void stopExecution();

    // printed actual value of source registers is value before execution
    // but it of destination registers is value after execution
    std::string dumpExecutedInstr();
};

#endif

