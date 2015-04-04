#include <iostream>
#include <func_sim.h>

void clock_fetch(int cycle){
        mem = new FuncMemory(tr.c_str());
        PC = mem->startPC();
        uint32 instr_bytes = fetch();

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

void clock_decode(int cycle){
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

void clock_execute(int cycle){
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

void clock_memory(int cycle){
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

void clock_writeback(int cycle){
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

PerfMIPS::run(char* filename, int instr_num) {
        // .. init
        executed_instrs = 0; // this variable is stored inside PerfMIPS class
        cycle = 0;
        while (executed_instr <= instrs_to_run) {
              fetch->clock(cycle);
              decode->clock(cycle);
              execute->clock(cycle);
              memory->clock(cycle);
              writeback->clock(cycle); // each instruction writeback increases executed_instrs variable
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
        fetch = new PerfMIPS_module(NULL, wp_fetch_2_decode, &fetch_clock);
        decode = new PerfMIPS_module(rp_fetch_2_decode, wp_decode_2_execute, &decode_clock);
        execute = new PerfMIPS_module(rp_decode_2_execute, wp_execute_2_memory, &execute_clock)
        memory = new PerfMIPS_module(rp_execute_2_memory, wp_memory_2_writeback, &memory_clock);
        writeback = new PerfMIPS_module(rp_memory_2_writeback, NULL, &writeback_clock);
}
