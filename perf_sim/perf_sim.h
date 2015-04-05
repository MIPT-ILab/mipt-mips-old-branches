/* 
 * perf_sim.h - header of performance simulator
 * for MIPS based microprocessors
 * @author Semyon Abramov abramov.semyon.mipt@gmail.com
 * iLab project MIPS-MIPS 2015
 */


 #ifndef PERF_SIM
#define PERF_SIM

// Generic C++
#include<iostream>
#include<string>

// MIPT-MIPS modules
#include"ports.h"
#include<elf_parser.h>
#include<func_memory.h>
#include<func_instr.h>
#include<types.h>
#include<rf.h>

#define PORT_LATENCY 1
#define PORT_FANOUT 1
#define PORT_BW 1

using namespace std;

struct Pc
{
	int value;
	bool is_valid_in_fetch;	// validation bits for j, jr, beq, bne
	bool is_valid_in_decode;
	Pc(): value( 0ull), is_valid_in_fetch( true), is_valid_in_decode( true){ } ;
	
};

class PerfMIPS
{
	private:
		
		int executed_instr;
		int cycle;
		bool silent_output; // bit for activation of silent output
		bool ready;         // this validate bit is used in decode 
		uint32 bytes;
		struct Pc* PC;
		RF* rf;
		FuncMemory* mem;
	
	public:
		
		PerfMIPS();
		~PerfMIPS();
		ReadPort< uint32>* rp_fetch_to_decode;
		WritePort< uint32>* wp_fetch_to_decode;
		ReadPort< FuncInstr>* rp_decode_to_execute;
		WritePort< FuncInstr>* wp_decode_to_execute;
		ReadPort< FuncInstr>* rp_execute_to_memory;
		WritePort< FuncInstr>* wp_execute_to_memory;
		ReadPort< FuncInstr>* rp_memory_to_writeback;
		WritePort< FuncInstr>* wp_memory_to_writeback;
		ReadPort< bool>* rp_decode_to_fetch_stall;
		WritePort< bool>* wp_decode_to_fetch_stall;
		ReadPort< bool>* rp_execute_to_decode_stall;
		WritePort< bool>* wp_execute_to_decode_stall;
		ReadPort< bool>* rp_memory_to_execute_stall;
		WritePort< bool>* wp_memory_to_execute_stall;
		ReadPort< bool>* rp_writeback_to_memory_stall;
		WritePort< bool>* wp_writeback_to_memory_stall;
		
		/* internal data of decode module
		to continue decode after stall's end*/
		FuncInstr* decode_instr;  
		
		/* internal data of execution module
		to continue execution after stall's end
		if loads' delay is not 1 clock */
		FuncInstr* exec_instr;
		
		/* internal data of memory module
		to continue memory stage after stall's end
		if loads' delay is not 1 clock */
		FuncInstr* mem_instr;
		
		/* Modules*/
		void clock_fetch( int cycle);
		void clock_decode( int cycle);		
		void clock_execute( int cycle);
		void clock_memory( int cycle);
		void clock_writeback( int cycle);
		
		void run( const std::string& tr, int instrs_to_run, bool silent);
		void read_src( FuncInstr& instr);  
		bool src_ready( FuncInstr& instr);
		
		void load( FuncInstr& instr) const 
		{
			instr.set_v_dst( mem->read( instr.get_mem_addr(), instr.get_mem_size()));
		}

		void store( const FuncInstr& instr) 
		{
			mem->write( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
		}

		void load_store( FuncInstr& instr) 
		{
			if ( instr.is_load())
			{    
				load( instr);
			}
			else if ( instr.is_store())
			{    
				store( instr);
			}
		}
		
		void memory_moduleDump( FuncInstr& instr )
		{
			instr.invalidateComplete( );
			cout << "memory\t" << "cycle " << cycle << ":\t" << instr << endl;
			instr.validateComplete();
		}
};


#endif // PERF_SIM