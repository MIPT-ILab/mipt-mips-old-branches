#include <iostream>
#include <func_sim.h>
#include <perf_sim.h>

void PerfMIPS::clock_fetch(int cycle){
        bool is_stall;
        /* If the next module tells us to stall, we stops
           and send stall signals to previous module */
        rp_decode_2_fetch_stall->read( &is_stall, cycle);
        if ( is_stall) return;

        wp_fetch_2_decode->write(fetch(), cycle);
}

void PerfMIPS::clock_decode(int cycle){
        //bool is_stall;
        /* If the next module tells us to stall, we stops
           and send stall signals to previous module */
        //rp_execute_2_decode_stall->read( &is_stall, cycle);
        /*if ( is_stall) {
             wp_decode_2_fetch_stall->write( true, cycle);
             return;
        }*/

        /* But, decode stage doesn't jump out
           It takes non-updated bytes from module_data
           and re-decodes them */
        rp_fetch_2_decode->read( &module_data, cycle)

        // Here we process data.
        bool succ_read_src = read_src(module_data);

        if (succ_read_src) {
             /* This branch is chosen if everything is OK and
                we may continue promotion to the next pipeline stages */
             wp_decode_2_execute->write( module_data, cycle);
        }
        else {
             // Otherwise, nothing is done and we have to stall pipeline
             wp_decode_2_fetch_stall->write( true, cycle);
        }
}

void PerfMIPS::clock_execute(int cycle){
        //bool is_stall;
        /* If the next module tells us to stall, we stops
           and send stall signals to previous module */
        /*rp_memory_2_execute_stall->read( &is_stall, cycle);
        if ( is_stall) {
             wp_execute_2_decode_stall->write( true, cycle);
             return;
        }*/

        FuncInstr module_data;
        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( rp_decode_2_execute->read( &module_data, cycle))
            return;

        // Here we process data.
        module_data.execute();
        wp_execute_2_memory->write( module_data, cycle);

        /*if (succ_read_src) {
            wp_execute_2_memory->write( module_data, cycle);
        }
        else {
            // Otherwise, nothing is done and we have to stall pipeline
            wp_execute_2_decode_stall->write( true, cycle);
        }*/
}

void PerfMIPS::clock_memory(int cycle){
        //bool is_stall;
        /* If the next module tells us to stall, we stops
           and send stall signals to previous module */
        /*rp_writeback_2_memory_stall->read( &is_stall, cycle);
        if ( is_stall) {
             wp_memory_2_execute_stall->write( true, cycle);
             return;
        }*/

        FuncInstr module_data;
        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( rp_execute_2_memory->read( &module_data, cycle))
            return;

        // Here we process data.
        load_store(module_data);
        wp_memory_2_writeback->write( module_data, cycle);

        /*if (...) {
             wp_memory_2_writeback->write( module_data, cycle);
        }
        else {
             // Otherwise, nothing is done and we have to stall pipeline
             wp_memory_2_execute_stall->write( true, cycle);
        }*/
}

void PerfMIPS::clock_writeback(int cycle){

        FuncInstr module_data;
        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( rp_memory_2_writeback->read( &module_data, cycle))
            return;

        // Here we process data.
        wb(module_data);

        executed_instrs++;
}

PerfMIPS::run(const std::string& tr, int instrs_to_run) {
        mem = new FuncMemory(tr.c_str());
        PC = mem->startPC();
        executed_instrs = 0; // this variable is stored inside PerfMIPS class
        cycle = 0;
        while (executed_instr <= instrs_to_run) {
              clock_fetch(cycle);
              clock_decode(cycle);
              clock_execute(cycle);
              clock_memory(cycle);
              clock_writeback(cycle); // each instruction writeback increases executed_instrs variable
              ++cycle;
        }
        // ..
    }

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
        /*fetch = new PerfMIPS_module(NULL, wp_fetch_2_decode, &fetch_clock);
        decode = new PerfMIPS_module(rp_fetch_2_decode, wp_decode_2_execute, &decode_clock);
        execute = new PerfMIPS_module(rp_decode_2_execute, wp_execute_2_memory, &execute_clock)
        memory = new PerfMIPS_module(rp_execute_2_memory, wp_memory_2_writeback, &memory_clock);
        writeback = new PerfMIPS_module(rp_memory_2_writeback, NULL, &writeback_clock);*/
}
