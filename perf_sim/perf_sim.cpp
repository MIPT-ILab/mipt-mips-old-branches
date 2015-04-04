#include <iostream>

#include <func_sim.h>


PerfMIPS::PerfMIPS() {
        // Ports initialization
        rp_fetch_2_decode = new ReadPort<uint32>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
        rp_decode_2_execute = new ReadPort<FuncInstr>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
        rp_execute_2_memory = new ReadPort<FuncInstr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
        rp_memory_2_writeback = new ReadPort<FuncInstr>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);

        wp_fetch_2_decode = new WritePort<uint32>("FETCH_2_DECODE", PORT_LATENCY);
        wp_decode_2_execute = new WritePort<FuncInstr>("DECODE_2_EXECUTE", PORT_LATENCY);
        wp_execute_2_memory = new WritePort<FuncInstr>("EXECUTE_2_MEMORY", PORT_LATENCY);
        wp_memory_2_writeback = new WritePort<FuncInstr>("MEMORY_2_WRITEBACK", PORT_LATENCY);

        // Stall ports initialization
        rp_decode_2_fetch_stall = new ReadPort<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
        wp_decode_2_fetch_stall = new ReadPort<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);

        // Init modules
        fetch = new PerfMIPS_module(NULL, wp_fetch_2_decode);
        decode = new PerfMIPS_module(rp_fetch_2_decode, wp_decode_2_execute);
        execute = new PerfMIPS_module(rp_decode_2_execute, wp_execute_2_memory)
        memory = new PerfMIPS_module(rp_execute_2_memory, wp_memory_2_writeback);
        writeback = new PerfMIPS_module(rp_memory_2_writeback, NULL);
    }
