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
#include <func_sim.h>
#include <types.h>

class PerfMIPS {

    void run(/* */);                               // start emulation
    PerfMIPS();                                    // prepare ports to work
    ~PerfMIPS();

    int executed_instrs;                           // number of executed instructions

    // fetch
    WritePort<uint32>* wp_fetch_2_decode;          // write fetch to decode
    
    ReadPort<bool>* rp_decode_2_fetch_stall;       // read decode to fetch stall
    
    FuncInstr* fetchInstr;                         // current instruction in fetch
    void clock_fetch( int cycle);                  // execute fetch

    // decode
    ReadPort<uint32>*     rp_fetch_2_decode;       // read fetch to decode
    WritePort<FuncInstr>* wp_decode_2_execute;     // write decode to execute

    ReadPort<bool>*  rp_execute_2_decode_stall;    // read execute to decode stall
    WritePort<bool>* wp_decode_2_fetch_stall;      // write decode to fetch stall

    FuncInstr* decodeInstr;                        // current instruction in decode
    void clock_decode( int cycle);                 // execute decode

    // execute
    ReadPort<FuncInstr>*  rp_decode_2_execute;     // read decode to execute
    WritePort<FuncInstr>* wp_execute_2_memory;     // write execute to memory

    ReadPort<bool>*  rp_memory_2_execute_stall;    // read memory to execute stall
    WritePort<bool>* wp_execute_2_decode_stall;    // write execute to decode stall

    FuncInstr* executeInstr;                       // current instruction in execute
    void clock_execute( int cycle);                // execute execute

    // memory
    ReadPort<FuncInstr>*  rp_execute_2_memory;     // read execute to memory
    WritePort<FuncInstr>* wp_memory_2_writeback;   // write memory to writeback

    ReadPort<bool>* rp_writeback_2_memory_stall;   // read writeback to memory stall
    WritePort<bool>* wp_memory_2_execute_stall;     // write memory to execute stall

    FuncInstr* memoryInstr;                        // current instruction in memory
    void clock_memory( int cycle);                 // execute memory

    // writeback
    ReadPort<FuncInstr>* rp_memory_2_writeback;    // read memory to writeback
    
    WritePort<bool>* wp_writeback_2_memory_stall;  // write writeback to memory stall

    FuncInstr* writebackInstr;                     // current instruction in writeback
    void clock_writeback( int cycle);              // execute writeback
};

#endif // PERF_SIM_H