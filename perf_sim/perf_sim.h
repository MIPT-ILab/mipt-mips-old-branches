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

generic <typename rp_p2m_type, wp_m2n_type>
class PerfMIPS_module {
    ReadPort<rp_p2m_type>* rp_previous_2_me;
    ReadPort<bool>* rp_next_2_me_stall;

    WritePort<wp_m2n_type>* wp_me_2_next;
    WritePort<bool>* wp_me_2_previous_stall;

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
        ReadPort<bool>* rp_decode_2_fetch_stall;
        WritePort<bool>* wp_decode_2_fetch_stall;
    };

#endif

