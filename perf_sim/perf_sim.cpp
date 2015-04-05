/**
 * perf_sim.cpp - Module implementing scalar MIPS simulator
 *                with constant latency for instruction execution
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

#include <perf_sim.h>


// prepare ports for working
PerfMIPS::PerfMIPS() {

    // init register file
    
    rf = new RF();

    // init data ports and main variables of MIPS

    executed_instrs = 0;   // set zero number of executed instructions

    // fetch
    wp_fetch_2_decode = new WritePort<uint32>( "FETCH_2_DECODE", PORT_BW, PORT_FANOUT);                    // init write fetch to decode
    
    fetchInstr = 0;        // undefined instruction in fetch

    // decode
    rp_fetch_2_decode = new ReadPort<uint32>( "FETCH_2_DECODE", PORT_LATENCY);                             // init read fetch to decode
    wp_decode_2_execute = new WritePort<FuncInstr>( "DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);             // init write decode to execute

    decodeInstr = NULL;    // undefined instruction in decode
    next_is_valid = false;

    // execute
    rp_decode_2_execute = new ReadPort<FuncInstr>( "DECODE_2_EXECUTE", PORT_LATENCY);                      // init read decode to execute
    wp_execute_2_memory = new WritePort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);             // init write execute to memory

    executeInstr = NULL;   // undefined instruction in execute

    // memory
    rp_execute_2_memory = new ReadPort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_LATENCY);                      // init read execute to memory
    wp_memory_2_writeback = new WritePort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);         // init write memory to writeback

    memoryInstr = NULL;    // undefined instruction in memory

    // writeback
    rp_memory_2_writeback = new ReadPort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_LATENCY);                  // init read memory to writeback

    writebackInstr = NULL; // undefined instruction in writeback

    // init stall ports

    wp_writeback_2_memory_stall = new WritePort<bool>( "WRITEBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);  // init write writeback to memory stall
    rp_writeback_2_memory_stall = new ReadPort<bool>( "WRITEBACK_2_MEMORY_STALL", PORT_LATENCY);           // init read writeback to memory stall

    wp_memory_2_execute_stall = new WritePort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);      // init write memory to execute stall
    rp_memory_2_execute_stall = new ReadPort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_LATENCY);               // init read memory to execute stall

    wp_execute_2_decode_stall = new WritePort<bool>( "EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);      // init write execute to decode stall
    rp_execute_2_decode_stall = new ReadPort<bool>( "EXECUTE_2_DECODE_STALL", PORT_LATENCY);               // init read execute to decode stall

    wp_decode_2_fetch_stall = new WritePort<bool>( "DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);          // init write decode to fetch stall
    rp_decode_2_fetch_stall = new ReadPort<bool>( "DECODE_2_FETCH_STALL", PORT_LATENCY);                   // init read decode to fetch stall
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

    // register file
    delete rf;

    // instructions
    if ( decodeInstr)
        delete decodeInstr;

    if ( executeInstr)
        delete executeInstr;

    if ( memoryInstr)
        delete memoryInstr;

    if ( writebackInstr)
        delete writebackInstr;
}

// run emulation
void PerfMIPS::run( const std::string& tr, uint32 instrs_to_run, bool silent) {
    Port<bool>::init();
    Port<FuncInstr>::init();
    Port<uint32>::init();

    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();

    is_silent = silent;
    int cycle = 0;

    while (executed_instrs < instrs_to_run) {
        clock_fetch( cycle);
        clock_decode( cycle);
        clock_execute( cycle);
        clock_memory( cycle);
        clock_writeback( cycle);
        ++cycle;
        if ( !is_silent) std::cout << std::endl;
    }
    delete mem;
}

/* Here is my implementation of pipeline blocks. Because of constant instructions length
   there is only one function generating stalls: decode. So in every module we have stall
   checks, but there is only one place where we can meet them.
 */

// fetch block
void PerfMIPS::clock_fetch( int cycle) {
    bool is_stall = false;

    rp_decode_2_fetch_stall->read( &is_stall, cycle);
    if ( is_stall || jump_ahead) {                                                                                            // if we have branch or jump instruction we have to wait until PC is ready
        if ( !is_silent) std::cout << "fetch\tclock " << cycle << ":  " << "bubble\n";
        return;
    }

    fetchInstr = fetch();

    wp_fetch_2_decode->write( fetchInstr, cycle);

    FuncInstr instr( fetchInstr, PC);                                                                                         // my model needs to know what operation is it 
    jump_ahead = instr.is_jump();                                                                                             // it can't halt wrong operations so there are no any predicts

    if ( !is_silent) std::cout << "fetch\tclock " << cycle << ":  " << std::hex << fetchInstr << std::dec << std::endl;
}

/* In best way, we need to implement every block like decode: with moduleNext, some stall checks etc.
   but because of constant length of functions I decided that it isn't neccessary here
 */

// decode block
void PerfMIPS::clock_decode( int cycle) {
    bool is_stall = false;

    rp_execute_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall) {
        wp_decode_2_fetch_stall->write( true, cycle);
        if ( !is_silent) std::cout << "decode\tclock " << cycle << ":  " << "bubble\n";
        return;
    }

    if ( rp_fetch_2_decode->read( &decodeNext, cycle))                                                                        // if we read data, this data is correct, if we didn't read - there are two variants:
        next_is_valid = true;                                                                                                 // - decodeNext has data that was given to it many cycles ago
                                                                                                                              // - we have no data to work with
    if ( !next_is_valid && !decodeInstr) {                                                                                    // We have no data to work with
        PC += 4;
        if ( !is_silent) std::cout << "decode\tclock " << cycle << ":  " << "bubble\n";
        return;
    }

    if ( !decodeInstr) {                                                                                                      // We decode data and work with it, next data is not new, so it is invalid
        decodeInstr = new FuncInstr( decodeNext, cycle);
        next_is_valid = false;
    }

    RegNum src1 = decodeInstr->get_src1_num();
    RegNum src2 = decodeInstr->get_src2_num();
    RegNum dst  = decodeInstr->get_dst_num();

    if ( rf->check( src1) && rf->check( src2)) {
        rf->invalidate( dst);
        read_src( *decodeInstr);
        wp_decode_2_execute->write( *decodeInstr, cycle);
        PC += 4;
        if ( !is_silent) std::cout << "decode\tclock " << cycle << ":  " << *decodeInstr << std::endl;
        delete decodeInstr;
        decodeInstr = NULL;                                                                                                   // It helps us in different check above
    }
    else {
        wp_decode_2_fetch_stall->write( true, cycle);
        if ( !is_silent) std::cout << "decode\tclock " << cycle << ":  " << "bubble\n";
    }
}

// execute block
void PerfMIPS::clock_execute( int cycle) {
    bool is_stall = false;
    rp_memory_2_execute_stall->read( &is_stall, cycle);
    if ( is_stall) {
        wp_execute_2_decode_stall->write( true, cycle);
        if ( !is_silent) std::cout << "execute\tclock " << cycle << ":  " << "bubble\n";
        return;
    }

    if ( executeInstr)
        delete executeInstr;
    executeInstr = new FuncInstr;
    
    if ( !rp_decode_2_execute->read( executeInstr, cycle)) {
        if ( !is_silent) std::cout << "execute\tclock " << cycle << ":  " << "bubble\n";
        return;
    }

    executeInstr->execute();
    
    if ( executeInstr->is_jump()) {
        PC = executeInstr->get_new_PC();
        jump_ahead = false;
    }

    wp_execute_2_memory->write( *executeInstr, cycle);
    if ( !is_silent) std::cout << "execute\tclock " << cycle << ":  " << *executeInstr << std::endl;
}

// memory block
void PerfMIPS::clock_memory( int cycle) {
    bool is_stall = false;
    rp_writeback_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall) {
        if ( !is_silent) std::cout << "memory\tclock " << cycle << ":  " << "bubble\n";
        wp_memory_2_execute_stall->write( true, cycle);
        return;
    }

    if ( memoryInstr)
        delete memoryInstr;
    memoryInstr = new FuncInstr;

    if ( !rp_execute_2_memory->read( memoryInstr, cycle)) {
        if ( !is_silent) std::cout << "memory\tclock " << cycle << ":  " << "bubble\n";
        return;
    }

    load_store( *memoryInstr);

    wp_memory_2_writeback->write( *memoryInstr, cycle);
    if ( !is_silent) std::cout << "memory\tclock " << cycle << ":  " << *memoryInstr << std::endl;
}

// writeback block
void PerfMIPS::clock_writeback( int cycle) {

    if ( writebackInstr)
        delete writebackInstr;
    writebackInstr = new FuncInstr;

    if ( !rp_memory_2_writeback->read( writebackInstr, cycle)) {
        return;
    }

    wb( *writebackInstr);

    executed_instrs++;
    if ( !is_silent) std::cout << std::endl;
    std::cout << ( *writebackInstr) << std::endl;
}
