/*******************************************************

    Interface for a scalar MIPS simulator

    @file: perf_sim.h
    @date: April 2, 2015
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @vertion: 1.0 (April 2, 2015)

*******************************************************/

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <perf_rf.h>
#include <ports.h>

#define PORT_BW 1
#define PORT_FANOUT 1
#define PORT_LATENCY 1

class MIPS
{
        RF* rf;
        uint32 PC;
	bool jump;
        FuncMemory* mem;
	int instr_num; //the same as instrs_to_run
	int read_instr;
	int executed_instr; //number of executed instuctions
	int cycle;

	uint32 fetch_raw; bool fetch_empty;
	uint32 decode_raw; bool decode_empty;
	FuncInstr exec_instr; bool exec_empty;

    //ports:
	    ReadPort<FuncInstr> *rp_decode_2_execute;
        ReadPort<FuncInstr> *rp_execute_2_memory;
        WritePort<uint32>   *wp_fetch_2_decode;
        WritePort<FuncInstr> *wp_decode_2_execute;
        WritePort<FuncInstr> *wp_execute_2_memory;
        WritePort<FuncInstr> *wp_memory_2_writeback;
        ReadPort<uint32> *rp_fetch_2_decode;
        ReadPort<FuncInstr> *rp_memory_2_writeback;

        ReadPort<bool>* rp_decode_2_fetch_stall;
        WritePort<bool>* wp_decode_2_fetch_stall;
        WritePort<bool>* wp_execute_2_decode_stall;
        WritePort<bool>* wp_memory_2_execute_stall;
        WritePort<bool>* wp_writeback_2_memory_stall;
        ReadPort<bool>* rp_execute_2_decode_stall;
        ReadPort<bool>* rp_memory_2_execute_stall;
        ReadPort<bool>* rp_writeback_2_memory_stall;

	void Fetch_Process(int cycle);
	void Decode_Process(int cycle);
	void Execute_Process(int cycle);
	void Memory_Process(int cycle);
	void Writeback_Process(int cycle);

        uint32 fetch() const { return mem->read(PC); }
        
	void read_src(FuncInstr& instr) const {
            rf->read_src1(instr); 
            rf->read_src2(instr); 
	}

        void load(FuncInstr& instr) const {
            instr.set_v_dst(mem->read(instr.get_mem_addr(), instr.get_mem_size()));
        }

        void store(const FuncInstr& instr) {
            mem->write(instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }

	void load_store(FuncInstr& instr) {
            if (instr.is_load())
                load(instr);
            else if (instr.is_store())
                store(instr);
        }

        void wb(const FuncInstr& instr) {
            rf->write_dst(instr);
        }
   public:
        MIPS();
        void run(const std::string& tr, uint32 instrs_to_run);
        ~MIPS();
};
            
#endif
 
