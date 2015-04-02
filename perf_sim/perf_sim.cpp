/**
 * perf_sim.cpp - Module implementing scalar MIPS simulator
 *                with constant latency for instruction execution
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

#include <perf_sim.h>


// prepare ports for working
PerfMIPS::PerfMIPS() {

    executed_instrs = 0;     // set zero number of executed instructions

    // fetch
    wp_fetch_2_decode = new WritePort<uint32>( "FETCH_2_DECODE", PORT_BW, PORT_FANOUT);                    // init write fetch to decode
    
    rp_decode_2_fetch_stall = new ReadPort<bool>( "DECODE_2_FETCH_STALL", PORT_LATENCY);                   // init read decode to fetch stall
    
    fetchInstr = NULL;       // undefined instruction in fetch

    // decode
    rp_fetch_2_decode = new ReadPort<uint32>( "FETCH_2_DECODE", PORT_LATENCY);                             // init read fetch to decode
    wp_decode_2_execute = new WritePort<FuncInstr>( "DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);             // init write decode to execute

    rp_execute_2_decode_stall = new ReadPort<bool>( "EXECUTE_2_DECODE_STALL", PORT_LATENCY);               // init read execute to decode stall
    wp_decode_2_fetch_stall = new WritePort<bool>( "DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);          // init write decode to fetch stall

    decodeInstr = NULL;      // undefined instruction in decode

    // execute
    rp_decode_2_execute = new ReadPort<FuncInstr>( "DECODE_2_EXECUTE", PORT_LATENCY);                      // init read decode to execute
    wp_execute_2_memory = new WritePort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);             // init write execute to memory

    rp_memory_2_execute_stall = new ReadPort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_LATENCY);               // init read memory to execute stall
    wp_execute_2_decode_stall = new WritePort<bool>( "EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);      // init write execute to decode stall

    executeInstr = NULL;     // undefined instruction in execute

    // memory
    rp_execute_2_memory = new ReadPort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_LATENCY);                      // init read execute to memory
    wp_memory_2_writeback = new WritePort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);         // init write memory to writeback

    rp_writeback_2_memory_stall = new ReadPort<bool>( "WRITEBACK_2_MEMORY_STALL", PORT_LATENCY);           // init read writeback to memory stall
    wp_memory_2_execute_stall = new WritePort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);      // init write memory to execute stall

    memoryInstr = NULL;     // undefined instruction in memory

    // writeback
    rp_memory_2_writeback = new ReadPort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_LATENCY);                  // init read memory to writeback
    
    wp_writeback_2_memory_stall = new WritePort<bool>( "WRITEBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);  // init write writeback to memory stall

    writebackInstr = NULL;  // undefined instruction in writeback
}

// free all allocated memory
PerfMIPS::~PerfMIPS() {
    // fetch
    delete wp_fetch_2_decode;

    delete rp_decode_2_fetch_stall;

    // decode
    delete rp_fetch_2_decode;
    delete wp_decode_2_execute;

    delete rp_execute_2_decode_stall;
    delete wp_decode_2_fetch_stall;

    // execute
    delete rp_decode_2_execute;
    delete wp_execute_2_memory;

    delete rp_memory_2_execute_stall;
    delete wp_execute_2_decode_stall;

    // memory
    delete rp_execute_2_memory;
    delete wp_memory_2_writeback;

    delete rp_writeback_2_memory_stall;
    delete wp_memory_2_execute_stall;

    // writeback
    delete rp_memory_2_writeback;
    
    delete wp_writeback_2_memory_stall;
}