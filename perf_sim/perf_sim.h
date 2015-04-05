/*
 * func_sim.cpp - mips single-cycle simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <ports.h>
#include <rf.h>

#define PORT_BW 1
#define PORT_FANOUT 1
#define PORT_LATENCY 1

/*
template <typename rp_p2m_type, wp_m2n_type>
class PerfMIPS_module {
    ReadPort<rp_p2m_type>* rp_previous_2_me;
    ReadPort<bool>* rp_next_2_me_stall;

    WritePort<wp_m2n_type>* wp_me_2_next;
    WritePort<bool>* wp_me_2_previous_stall;

    void *(*clock_module)(int *);
    void clock_module();


    PerfMIPS_module(ReadPort<rp_p2m_type>* rp_p2m,
                    WritePort<wp_m2n_type>* wp_m2n,
                    ReadPort<bool>* rp_n2m_s,
                    WritePort<bool>* wp_m2p_s,
                    void *(*clock_module_init)(int *)){
        rp_previous_2_me = rp_p2m;
        wp_me_2_next = wp_m2n;

        rp_next_2_me_stall = rp_n2m_s;
        wp_me_2_previous_stall = wp_m2p_s;
    }

    void clock(int cycle){

    }

};
*/

class PerfMIPS {
        RF* rf;
        uint32 PC;
        bool PC_is_valid;
        FuncMemory* mem;

        bool silent;

        int cycle;
        int executed_instrs;
        int num_of_instructions;

        uint32 fetch(bool* is_valid) const {
            *is_valid = PC_is_valid;
            return mem->read(PC);
        }

        void invalidate_PC(){ PC_is_valid = false;}
        void update_PC(uint32 new_PC){
            PC = new_PC;
            PC_is_valid = true;
        }


        bool read_src(FuncInstr& instr) const {
            return rf->read_src1(instr) && rf->read_src2(instr);
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

        void clock_fetch( int cycle);
        void clock_decode( int cycle);
        void clock_execute( int cycle);
        void clock_memory( int cycle);
        void clock_writeback( int cycle);

        // Ports
        ReadPort<uint32>*       rp_fetch_2_decode;
        ReadPort<FuncInstr>*    rp_decode_2_execute;
        ReadPort<FuncInstr>*    rp_execute_2_memory;
        ReadPort<FuncInstr>*    rp_memory_2_writeback;

        WritePort<uint32>*      wp_fetch_2_decode;
        WritePort<FuncInstr>*   wp_decode_2_execute;
        WritePort<FuncInstr>*   wp_execute_2_memory;
        WritePort<FuncInstr>*   wp_memory_2_writeback;

        // Stall ports
        ReadPort<bool>*     rp_decode_2_fetch_stall;
        /*ReadPort<bool>*     rp_execute_2_decode_stall;
        ReadPort<bool>*     rp_memory_2_execute_stall;
        ReadPort<bool>*     rp_writeback_2_memory_stall;*/

        WritePort<bool>*    wp_decode_2_fetch_stall;
        /*WritePort<bool>*    wp_execute_2_decode_stall;
        WritePort<bool>*    wp_memory_2_execute_stall;
        WritePort<bool>*    wp_writeback_2_memory_stall;*/

        // Modules
        /*PerfMIPS_module<uint32, uint32>*        fetch;
        PerfMIPS_module<uint32, FuncInstr>*     decode;
        PerfMIPS_module<FuncInstr, FuncInstr>*  execute;
        PerfMIPS_module<FuncInstr, FuncInstr>*  memory;
        PerfMIPS_module<FuncInstr, FuncInstr>*  writeback;*/
public:
        PerfMIPS();
        void run(const std::string& tr, int instrs_to_run, bool silent_mode = true);

};

#endif
