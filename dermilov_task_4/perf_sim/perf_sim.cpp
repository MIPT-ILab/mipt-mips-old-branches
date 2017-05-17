/*
 * perf_sim.cpp - mips scalar simulator
 * @author Dmitry Ermilov ermilov@phystech.edu
 * For MIPT-MIPS 2015
 */


#include <iostream>
#include <sstream>
#include <iomanip>

#include <perf_sim.h>

#define PORT_BW        1
#define PORT_FANOUT    1
#define PORT_LATENCY   1


PerfMIPS::PerfMIPS() : execute_obj(0ull, 0), memory_obj(0ull, 0), writeback_obj(0ull, 0), decode_obj(0ull, 0)
{
    rf = new RF();
    
    fetch_data = 0;
    decode_data = 0;  
    prev_dec = true;
    prev_exe = true;
    prev_mem = true;
    prev_wb = true;

    wp_fetch_2_decode = new WritePort<uint32>( "FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_fetch_2_decode = new ReadPort<uint32>( "FETCH_2_DECODE", PORT_LATENCY);
    wp_decode_2_execute = new WritePort<FuncInstr>( "DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = new ReadPort<FuncInstr>( "DECODE_2_EXECUTE", PORT_LATENCY);
    wp_execute_2_memory = new WritePort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = new ReadPort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_memory_2_writeback = new WritePort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_memory_2_writeback = new ReadPort<FuncInstr>( "MEMORY_2_WRITEBACK", PORT_LATENCY);


    wp_decode_2_fetch_stall = new WritePort<bool>( "DECODE_2_FETCH", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = new ReadPort<bool>( "DECODE_2_FETCH", PORT_LATENCY); 
    wp_execute_2_decode_stall = new WritePort<bool>( "EXECUTE_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_execute_2_decode_stall = new ReadPort<bool>( "EXECUTE_2_DECODE", PORT_LATENCY);
    wp_memory_2_execute_stall = new WritePort<bool>( "MEMORY_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_memory_2_execute_stall = new ReadPort<bool>( "MEMORY_2_EXECUTE", PORT_LATENCY);
    wp_writeback_2_memory_stall = new WritePort<bool>( "WRITEBACK_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_writeback_2_memory_stall = new ReadPort<bool>( "WRITEBACK_2_MEMORY", PORT_LATENCY);

}

void  PerfMIPS::run(const std::string& tr, uint32 instrs_to_run, bool silent) 
{
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    PC_is_valid = true;
    is_silent = silent;
    executed_instrs = 0; 
    uint32 cycle = 0;
    while (executed_instrs < instrs_to_run) {
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
    RegNum src1_reg = obj.get_src1_num();
    RegNum src2_reg = obj.get_src2_num();
    return ( rf->check( src1_reg) && rf->check( src2_reg));
}

bool  PerfMIPS::dst_check( FuncInstr obj) const
{
    RegNum dst_reg = obj.get_dst_num();
    return rf->check( dst_reg);
}

std::string PerfMIPS::fetch_dump( int cycle)
{
    std::ostringstream oss;
    oss << "fetch\tcycle " << cycle << " :\t" << "0x" << hex << fetch_data
        << dec;
    return oss.str();
}

std::string PerfMIPS::master_dump( std::string module, std::string disasm, 
                                   int cycle )
{
    ostringstream oss;
    oss << module << "\tcycle " << cycle << " :\t" << disasm;
    return oss.str();
}

void  PerfMIPS::clock_fetch( int cycle) 
{
//    cerr << "fetch\t"; 
    if  ( PC_is_valid == false )
        return;

    bool is_stall;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);

//    cerr << "stall = " << is_stall << "\n";
    if  ( !is_stall) {
        fetch_data = fetch();
        if  ( !is_silent) {
            std::cout << fetch_dump( cycle ) << endl;
        }
        wp_fetch_2_decode->write( fetch_data, cycle);
    }
}

 void  PerfMIPS::clock_decode( int cycle) 
 {
//    cerr << "decode\t";
    bool is_stall;

    rp_execute_2_decode_stall->read( &is_stall, cycle);
//    cerr << "stall = " << is_stall << "\n";
    if ( is_stall) {
        wp_decode_2_fetch_stall->write( true, cycle);
        return;
    }

    if  ( prev_dec == true) {
        if ( rp_fetch_2_decode->read( &decode_data, cycle)) {
            FuncInstr tmp_obj( decode_data, PC);
            decode_obj = tmp_obj;
        }
        else {
            return;
        }
    }
    else {
        FuncInstr tmp_obj( decode_data, PC);
        decode_obj = tmp_obj;
    }
    
    if  ( decode_obj.is_jump())
        PC_is_valid = false;
    else
        PC = decode_obj.get_new_PC();

    prev_dec = false;
    if  (  ( src_check( decode_obj) == true) 
        && ( dst_check( decode_obj) == true )) {
//        cerr << "+++\n";
        read_src( decode_obj);
        rf->invalidate( decode_obj.get_dst_num());
        prev_dec = true;
        wp_decode_2_execute->write( decode_obj, cycle);

        if  ( !is_silent) {
            std::cout << master_dump( "decode", decode_obj.Dump(), cycle ) 
                      << endl;
        }    
    }
    else  {
//        cerr << "---\n";
        wp_decode_2_fetch_stall->write( true, cycle);
    }
}            

void  PerfMIPS::clock_execute( int cycle) 
{
//    cerr << "execute\t";
    bool is_stall;
    rp_memory_2_execute_stall->read( &is_stall, cycle);
//    cerr << "stall = " << is_stall << "\n";
    if ( is_stall) {
        wp_execute_2_decode_stall->write( true, cycle);
        return;
    }

    if  ( prev_exe == true) {
        if ( rp_decode_2_execute->read( &execute_obj, cycle) == false)
            return;
    }
        execute_obj.execute();

    if ( execute_obj.is_complete()) {
//        cerr << "+++\n";
        prev_exe = true;
        wp_execute_2_memory->write( execute_obj, cycle);

        if  ( !is_silent) {
            std::cout << master_dump( "execute", execute_obj.Dump(), cycle ) 
                      << endl;
        }
    }
    else {
//        cerr << "---\n";
        prev_exe = false;
        wp_execute_2_decode_stall->write( true, cycle);
    }
}            

void  PerfMIPS::clock_memory( int cycle) 
{
//    cerr << "memory\t"; 
    bool is_stall;
    rp_writeback_2_memory_stall->read( &is_stall, cycle);
//    cerr << "stall = " << is_stall << "\n";
    if ( is_stall) {
        wp_memory_2_execute_stall->write( true, cycle);
        return;
    }

    if  ( prev_mem == true) {
        if ( rp_execute_2_memory->read( &memory_obj, cycle) == false)
            return;
    }

    if  ( ( memory_obj.get_dst_num() == 0) || dst_check( memory_obj) == false) {
//        cerr << "+++\n";
        load_store( memory_obj);
        prev_mem = true;
        wp_memory_2_writeback->write( memory_obj, cycle);

        if  ( !is_silent) {
            std::cout << master_dump( "memory", memory_obj.Dump(), cycle )
                      << endl;
        }
    }
    else {
        prev_mem = false;
        wp_memory_2_execute_stall->write( true, cycle);
    }
}

void  PerfMIPS::clock_writeback( int cycle) 
{
//    cerr << "writeback\n";
    if  ( prev_wb == true) {
        if  ( rp_memory_2_writeback->read( &writeback_obj, cycle) == false)
            return;
    }

    if  ( dst_check( writeback_obj) == false) {
//        cerr << "+++\n";
        wb( writeback_obj);
        executed_instrs++;
        prev_wb = true;

        if  ( !is_silent) {
            std::cout << master_dump( "writeback", writeback_obj.Dump(), cycle )
                      << endl;
        }
        else {
            std::cout << writeback_obj.Dump() << endl;
        }

        if  ( writeback_obj.is_jump()) {
            PC = writeback_obj.get_new_PC();
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
