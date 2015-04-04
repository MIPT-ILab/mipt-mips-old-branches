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
#include <perf_module.h>

#define PORT_BW 1
#define PORT_FANOUT 1
#define PORT_LATENCY 1

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
        ReadPort<bool>*     rp_fetch_2_decode_stall;
        ReadPort<bool>*     rp_decode_2_execute_stall;
        ReadPort<bool>*     rp_execute_2_memory_stall;
        ReadPort<bool>*     rp_memory_2_writeback_stall;

        WritePort<bool>*    wp_fetch_2_decode_stall;
        WritePort<bool>*    wp_decode_2_execute_stall;
        WritePort<bool>*    wp_execute_2_memory_stall;
        WritePort<bool>*    wp_memory_2_writeback_stall;

        // Modules
        PerfMIPS_module<uint32, uint32>         fetch;
        PerfMIPS_module<uint32, FuncInstr>      decode;
        PerfMIPS_module<FuncInstr, FuncInstr>   execute;
        PerfMIPS_module<FuncInstr, FuncInstr>   memory;
        PerfMIPS_module<FuncInstr, FuncInstr>   writeback;
};

#endif
