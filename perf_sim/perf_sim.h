/**
 * perf_sim.h - Header of module implementing scalar MIPS simulator
 *              with constant latency for instruction execution
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#define PORT_LATENCY 1
#define PORT_BW 1
#define PORT_FANOUT 1

#include <ports.h>
#include <func_instr.h>
#include <func_mem.h>
#include <types.h>
#include <rf.h>

class PerfMIPS {
        RF* rf;                                        // registers
        uint32 PC;                                     // current PC
        FuncMemory* mem;                               // memory

        int executed_instrs;                           // number of executed instructions

        // fetch
        WritePort<uint32>* wp_fetch_2_decode;          // write fetch to decode
    
        ReadPort<bool>* rp_decode_2_fetch_stall;       // read decode to fetch stall
    
        FuncInstr* fetchInstr;                         // current instruction in fetch
        void clock_fetch( int cycle);                  // execute fetch block

        uint32 fetch() const { return mem->read(PC); } // execute fetch

        // decode
        ReadPort<uint32>*     rp_fetch_2_decode;       // read fetch to decode
        WritePort<FuncInstr>* wp_decode_2_execute;     // write decode to execute

        ReadPort<bool>*  rp_execute_2_decode_stall;    // read execute to decode stall
        WritePort<bool>* wp_decode_2_fetch_stall;      // write decode to fetch stall

        FuncInstr* decodeInstr;                        // current instruction in decode
        void clock_decode( int cycle);                 // execute decode block

        void read_src( FuncInstr& instr) const {       // read sources
            size_t reg_num = instr.get_src1_num();
            instr.set_v_src1( rf->read( ( Reg_Num) reg_num));

            reg_num = instr.get_src2_num();
            instr.set_v_src2( rf->read( ( Reg_Num) reg_num));
	    }

        // execute
        ReadPort<FuncInstr>*  rp_decode_2_execute;     // read decode to execute
        WritePort<FuncInstr>* wp_execute_2_memory;     // write execute to memory

        ReadPort<bool>*  rp_memory_2_execute_stall;    // read memory to execute stall
        WritePort<bool>* wp_execute_2_decode_stall;    // write execute to decode stall

        FuncInstr* executeInstr;                       // current instruction in execute
        void clock_execute( int cycle);                // execute execute block

        // memory
        ReadPort<FuncInstr>*  rp_execute_2_memory;     // read execute to memory
        WritePort<FuncInstr>* wp_memory_2_writeback;   // write memory to writeback

        ReadPort<bool>* rp_writeback_2_memory_stall;   // read writeback to memory stall
        WritePort<bool>* wp_memory_2_execute_stall;    // write memory to execute stall

        FuncInstr* memoryInstr;                        // current instruction in memory
        void clock_memory( int cycle);                 // execute memory block

        void load( FuncInstr& instr) const {           // load data
            instr.set_v_dst( mem->read( instr.get_mem_addr(), instr.get_mem_size()));
        }

        void store( const FuncInstr& instr) {          // store data
            mem->write( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }

    	void load_store( FuncInstr& instr) {           // work with memory
            if ( instr.is_load())
                load( instr);
            else if ( instr.is_store())
                store( instr);
        }

        // writeback
        ReadPort<FuncInstr>* rp_memory_2_writeback;    // read memory to writeback
    
        WritePort<bool>* wp_writeback_2_memory_stall;  // write writeback to memory stall

        FuncInstr* writebackInstr;                     // current instruction in writeback
        void clock_writeback( int cycle);              // execute writeback block

        void wb( const FuncInstr& instr) {             // do writeback
            rf->write_dst( instr);
        }

    public:
        // start
        void run( const std::string& tr, uint32 instrs_to_run, bool silent);
        
        // prepare ports to work
        PerfMIPS();

        // free allocated memory
        ~PerfMIPS();

};

#endif // PERF_SIM_H