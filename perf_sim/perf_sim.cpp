#include <iostream>
#include <perf_sim.h>
#include <cstring>

void PerfMIPS::clock_fetch(int cycle){
        bool is_stall = false;
        /* If the next module tells us to stall, we stops
           and send stall signals to previous module */
        rp_decode_2_fetch_stall->read( &is_stall, cycle);
        if ( is_stall) return;
        bool succ_fetch;
        uint32 raw_instr = fetch(&succ_fetch);
        if(succ_fetch) wp_fetch_2_decode->write(raw_instr, cycle);
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
        uint32 decode_data;

        if ( !rp_fetch_2_decode->read( &decode_data, cycle))
            return;

        FuncInstr* curr_instr = new FuncInstr(decode_data, PC);
        if(curr_instr->is_jump()) invalidate_PC();

        // Here we process data.
        bool succ_read_src = read_src(*curr_instr);

        if (succ_read_src && !curr_instr->is_jump()) {
             /* This branch is chosen if everything is OK and
                we may continue promotion to the next pipeline stages */
             wp_decode_2_execute->write( *curr_instr, cycle);
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

        FuncInstr* executing_instr = new FuncInstr();
        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( !rp_decode_2_execute->read( executing_instr, cycle))
            return;

        // Here we process data.
        executing_instr->execute();
        wp_execute_2_memory->write( *executing_instr, cycle);
        update_PC(executing_instr->get_new_PC());

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

        FuncInstr* memory_instr = new FuncInstr;
        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( !rp_execute_2_memory->read( memory_instr, cycle))
            return;

        // Here we process data.
        load_store(*memory_instr);
        wp_memory_2_writeback->write( *memory_instr, cycle);

        /*if (...) {
             wp_memory_2_writeback->write( module_data, cycle);
        }
        else {
             // Otherwise, nothing is done and we have to stall pipeline
             wp_memory_2_execute_stall->write( true, cycle);
        }*/
}

void PerfMIPS::clock_writeback(int cycle){

        FuncInstr* writeback_instr = new FuncInstr;
        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( !rp_memory_2_writeback->read( writeback_instr, cycle))
            return;

        // Here we process data.
        wb(*writeback_instr);

        executed_instrs++;
}

void PerfMIPS::run(const std::string& tr, int instrs_to_run) {
        Port<bool>::init();
        Port<FuncInstr>::init();
        Port<uint32>::init();


        mem = new FuncMemory(tr.c_str());
        PC = mem->startPC();
        PC_is_valid = true;
        executed_instrs = 0; // this variable is stored inside PerfMIPS class
        cycle = 0;
        while (executed_instrs <= instrs_to_run) {
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
        rp_fetch_2_decode = new ReadPort<uint32>("FETCH_2_DECODE", PORT_LATENCY);
        rp_decode_2_execute = new ReadPort<FuncInstr>("DECODE_2_EXECUTE", PORT_LATENCY);
        rp_execute_2_memory = new ReadPort<FuncInstr>("EXECUTE_2_MEMORY", PORT_LATENCY);
        rp_memory_2_writeback = new ReadPort<FuncInstr>("MEMORY_2_WRITEBACK", PORT_LATENCY);

        wp_fetch_2_decode = new WritePort<uint32>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
        wp_decode_2_execute = new WritePort<FuncInstr>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
        wp_execute_2_memory = new WritePort<FuncInstr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
        wp_memory_2_writeback = new WritePort<FuncInstr>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);

        // Stall ports initialization
        rp_decode_2_fetch_stall = new ReadPort<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);
        wp_decode_2_fetch_stall = new WritePort<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);

        rf = new RF();
        // Init modules
        /*fetch = new PerfMIPS_module(NULL, wp_fetch_2_decode, &fetch_clock);
        decode = new PerfMIPS_module(rp_fetch_2_decode, wp_decode_2_execute, &decode_clock);
        execute = new PerfMIPS_module(rp_decode_2_execute, wp_execute_2_memory, &execute_clock)
        memory = new PerfMIPS_module(rp_execute_2_memory, wp_memory_2_writeback, &memory_clock);
        writeback = new PerfMIPS_module(rp_memory_2_writeback, NULL, &writeback_clock);*/
}
