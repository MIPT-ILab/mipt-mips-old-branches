/*
 * perf_sim.cpp - mips scalar simulator
 * @author Dmitry Ermilov ermilov@phystech.edu
 * For MIPT-MIPS 2015
 */

#include <iostream>

#include <perf_sim.h>

#define PORT_BW        1
#define PORT_FANOUT    1
#define PORT_LATENCY   1


PerfMIPS::PerfMIPS( ) 
{
    rf = new RF();
    
    wp_fetch_2_decode = new WritePort<FuncInstr>( "FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_fetch_2_decode = new ReadPort<FuncInstr>( "FETCH_2_DECODE", PORT_LATENCY);
    wp_decode_2_execute = new WritePort<FuncInstr>( "DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = new ReadPort<FuncInstr>( "DECODE_2_EXECUTE", PORT_LATENCY);
    wp_execute_2_memory = new WritePort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = new ReadPort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_memory_2_writeback = new WritePort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_memory_2_writeback = new ReadPort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_LATENCY);


    wp_decode_2_fetch_stall = new WritePort<FuncInstr>( "DECODE_2_FETCH", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = new ReadPort<FuncInstr>( "DECODE_2_FETCH", PORT_LATENCY); 
    wp_execute_2_decode_stall = new WritePort<FuncInstr>( "EXECUTE_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_execute_2_decode_stall = new ReadPort<FuncInstr>( "EXECUTE_2_DECODE", PORT_LATENCY);
    wp_memory_2_execute_stall = new WritePort<FuncInstr>( "MEMORY_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_memory_2_execute_stall = new ReadPort<FuncInstr>( "EMEMORY_2_EXECUTE", PORT_LATENCY);
    wp_writeback_2_memory_stall = new WritePort<FuncInstr>( "WRITEBACK_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_writeback_2_memory_stall = new ReadPort<FuncInstr>( "WRITEBACK_2_MEMORY", PORT_LATENCY);

}

void  PerfMIPS::run(const std::string& tr, uint32 instrs_to_run, bool silent) 
{
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    PC_is_valid = true;
    is_silent = silent;
    executed_instrs = 0; 
    cycle = 0;
    while (executed_instr <= instrs_to_run) {
        clock_fetch(cycle);
        clock_decode(cycle);
        clock_execute(cycle);
        clock_memory(cycle);
        clock_writeback(cycle);
        ++cycle;
    }
    delete mem;
}

bool  PerfMIPS::src_check( FuncInstr obj) const
{
    size_t src1_reg = obj.get_src1_num();
    size_t src2_reg = obj.get_src2_num();
    return ( rf->check( src1_reg) && rf->check( src2_reg));
}

bool  PerfMIPS::dst_check( FuncInstr obj) const
{
    size_t dst_reg = decode_obj.get_dst_num();
    return rf->check( dst_reg);
}

std::string PerfMIPS::fetch_dump( int cycle)
{
    ostringstream oss;
    if  ( !is_silent)
        oss << "fetch\tcycle " << cycle << " :\t";
    oss << "0x" << hex << fetch_data << dec;
    return oss.str();
}

std::string PerfMIPS::pref_dump( std::string module, std::string disasm, int cycle );
{
    ostringstream oss;
    if  ( !is_silent)
        oss << module << "\tcycle " << cycle << " :\t";
    oss << disasm;
    return oss.str();
}

void  PerfMIPS::clock_fetch( int cycle) 
{
    if  ( PC_is_valid == false )
        return;

    bool is_stall;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);

    if  ( !is_stall) {
        fetch_data = fetch();
        std::cout << fetch_dump( cycle ) << endl;
        wp_fetch_2_decode->write( fetch_data, cycle);
    }
}

 void  PerfMIPS::clock_decode( int cycle) 
 {
    bool is_stall;

    rp_execute_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall) {
        wp_decode_2_fetch_stall->write( true, cycle);
        return;
    }

    if  ( prev_dec == true) {
        if ( rp_fetch_2_decode->read( &decode_data, cycle)) {
            FuncInstr tmp_instr( decode_data, PC);
            decode_obj = tmp_instr;
        }
        else {
            return;
        }
    }
    else {
        FuncInstr tmp_instr( decode_data, PC);
        decode_obj = tmp_instr;
    }
    
    if  ( decode_obj.is_jump())
        PC_is_valid = false;
    else
        PC = decode_obj.get_new_PC();

    prev_dec = false;
    if  (  ( src_check( decode_obj) == true) 
        && ( dst_check( decode_obj) == true )) {
        read_src( decode_obj);
        std::cout << perf_dump( "decode", decode_obj.disasm, cycle ) << endl;
        rf->invalidate( dst_reg);
        prev_dec = true;
        wp_decode_2_execute->write( decode_obj, cycle);
    }
    else  {
        wp_decode_2_fetch_stall->write( true, cycle);
    }
}            

void  PerfMIPS::clock_execute( int cycle) 
{
    bool is_stall;
    rp_memory_2_execute_stall->read( &is_stall, cycle);
    if ( !is_stall) {
        wp_execute_2_decode_stall->write( true, cycle);
        return;
    }

    if  ( prev_exe == true) {
        if ( rp_decode_2_execute->read( &execute_obj, cycle) == false)
            return;
    }


    if  ( dst_check( execute_obj) == false) {
        execute_obj.execute();
    }
    
    if ( execute_obj.complete) {
        prev_exe = true;
        std::cout << perf_dump( "execute", execute_obj.disasm, cycle ) << endl;
        wp_execute_2_memory->write( execute_obj, cycle);
    }
    else {
        prev_exe = false;
        wp_execute_2_decode_stall->write( true, cycle);
    }
}            

void  PerfMIPS::clock_memory( int cycle) 
{
    bool is_stall;
    rp_writeback_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall) {
        wp_memory_2_execute_stall->write( true, cycle);
        return;
    }

    if  ( prev_mem == true) {
        if ( rp_execute_2_memory->read( &memory_obj, cycle) == false)
            return;
    }

    if  ( dst_check( memory_obj) == false) {
        load_store( memory_obj);
        prev_mem = true;
        std::cout << perf_dump( "memory", memory_obj.disasm, cycle ) << endl;
        wp_memory_2_writeback->write( memory_obj, cycle);
    }
    else {
        prev_mem = false;
        wp_memory_2_execute_stall->write( true, cycle);
    }
}

void  PerfMIPS::clock_writeback( int cycle) 
{
    if  ( prev_wb == true) {
        if  ( rp_memory_2_writeback->read( &writeback_obj, cycle) == false)
            return;
    }

    if  ( dst_check( writebac_obj) == false) {
        wb( writeback_obj);
        executed_instrs++;
        prev_wb = true;
        std::cout << perf_dump( "writeback", writeback_obj.disasm, cycle )
                  << endl;
        if  ( writebac_obj.is_jump()) {
            PC = writebac_obj.get_new_PC;
            PC_is_valid = true;
        }    
    }
    else {
        prev_wb = false;
        wp_writeback_2_memory_stall->write( true, cycle);
    }
}                    


PerfMIPS::~PerfMIPS() 
{
    delete rf;
    delete wp_fetch_2_decode;
    delete rp_fetch_2_decode;
    delete wp_decode_2_execute;
    delete rp_decode_2_execute;
    delete wp_execute_2_memory;
    delete rp_execute_2_memory;
    delete wp_memory_2_writeback;
    delete rp_memory_2_writeback;
    delete rp_decode_2_fetch_stall;
    delete wp_decode_2_fetch_stall;
    delete rp_execute_2_decode_stall;
    delete wp_execute_2_decode_stall;
    delete rp_memory_2_execute_stall;
    delete wp_memory_2_execute_stall;
    delete rp_writeback_2_memory_stall;
    delete wp_writeback_2_memory_stall;
}
