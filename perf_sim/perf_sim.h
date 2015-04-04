/*
 * func_sim.cpp - mips single-cycle simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <rf.h>

void fetch( int cycle) {
        bool is_stall;
        /* If the next module tells us to stall, we stops
           and send stall signals to previous module */
        rp_next_2_me_stall->read( &is_stall, cycle);
        if ( is_stall) {
             wp_me_2_previous_stall->write( true, cycle);
             return;
        }

        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( rp_previous_2_me->read( &module_data, cycle))
            return;

        /* But, decode stage doesn't jump out
           It takes non-updated bytes from module_data
           and re-decodes them */
        // rp_previous_2_me->read( &module_data, cycle)

        // Here we process data.

        if (...) {
             /* This branch is chosen if everything is OK and
                we may continue promotion to the next pipeline stages */
             wp_me_2_next->write( module_data, cycle);
        }
        else {
             // Otherwise, nothing is done and we have to stall pipeline
             wp_me_2_previous_stall->write( true, cycle);
        }
}


template <typename rp_p2m_type, wp_m2n_type>
class PerfMIPS_module {
    ReadPort<rp_p2m_type>* rp_previous_2_me;
    ReadPort<bool>* rp_next_2_me_stall;

    WritePort<wp_m2n_type>* wp_me_2_next;
    WritePort<bool>* wp_me_2_previous_stall;

    void *(*clock_module)(int *);

    PerfMIPS_module(ReadPort<rp_p2m_type>* rp_previous_2_me_init,
                    WritePort<wp_m2n_type>* wp_me_2_next_init,
                    void *(*clock_module_init)(int *)){
        rp_previous_2_me = rp_previous_2_me_init;
        wp_me_2_next = wp_me_2_next_init;

        clock_module = clock_module_init;
    }

};


class PerfMIPS {
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
        WritePort<bool>*    wp_decode_2_fetch_stall;

        // Modules
        PerfMIPS_module<uint32, uint32>         fetch;
        PerfMIPS_module<uint32, FuncInstr>      decode;
        PerfMIPS_module<FuncInstr, FuncInstr>   execute;
        PerfMIPS_module<FuncInstr, FuncInstr>   memory;
        PerfMIPS_module<FuncInstr, FuncInstr>   writeback;
};

#endif

