#include <iostream>
#include <perf_sim.h>


PerfMIPS::PerfMIPS():
    executed_instr(0),
    cycle(0),
    silent_mode(false)
{
    rf = new RF();

    rp_fetch_2_decode = new ReadPort<uint32>( "FETCH_2_DECODE", PORT_LATENCY);
    wp_fetch_2_decode = new WritePort<uint32>( "FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = new ReadPort<FuncInstr>( "DECODE_2_EXECUTE", PORT_LATENCY);
    wp_decode_2_execute = new WritePort<FuncInstr>( "DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = new ReadPort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_execute_2_memory = new WritePort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_memory_2_wback = new ReadPort<FuncInstr>( "MEMORY_2_WBACK", PORT_LATENCY);
    wp_memory_2_wback = new WritePort<FuncInstr>( "MEMORY_2_WBACK", PORT_BW, PORT_FANOUT);
    
    rp_wback_2_memory_stall = new ReadPort<bool>( "WBACK_2_MEMORY_STALL", PORT_LATENCY);
    wp_wback_2_memory_stall = new WritePort<bool>( "WBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);;
    rp_memory_2_execute_stall = new ReadPort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_LATENCY);
    wp_memory_2_execute_stall = new WritePort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
    rp_execute_2_decode_stall = new ReadPort<bool>( "EXECUTE_2_DECODE_STALL", PORT_LATENCY);
    wp_execute_2_decode_stall = new WritePort<bool>( "EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = new ReadPort<bool>( "DECODE_2_FETCH_STALL", PORT_LATENCY);
    wp_decode_2_fetch_stall = new WritePort<bool>( "DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);


}

void PerfMIPS::clock_fetch( uint64 cycle)
{
    bool is_stall;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);
    if ( is_stall) 
    {
        if ( !silent_mode) cout << "fetch   cycle " << cycle <<":  " << "stall" << endl;
        return;
    }
    if ( PC_is_valid)
    {
        uint32 instr_bytes = mem->read(PC);
        if ( !silent_mode) cout << "fetch   cycle " << cycle <<":  " << hex << instr_bytes << dec << endl;
        wp_fetch_2_decode->write( instr_bytes, cycle); 
    }
}


void PerfMIPS::clock_decode( uint64 cycle)
{
    bool is_stall;
    rp_execute_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        if ( !silent_mode) cout << "decode  cycle " << cycle <<":  " << "stall" << endl;
        wp_decode_2_fetch_stall->write( true, cycle);
        return;
    }
    uint32 instr_bytes = 0;
    rp_fetch_2_decode->read( &instr_bytes, cycle);
    FuncInstr instr( instr_bytes);
    if ( !silent_mode) cout << "decode  cycle " << cycle <<":  " << instr << endl;
    if ( PC_is_valid)
    {   
        if ( !read_src(instr))
        {
            wp_decode_2_fetch_stall->write( true, cycle);
            return;
        }
        rf->invalidate( instr.get_dst_num());
        PC_is_valid = instr.is_jump() ? true : false;
        if ( PC_is_valid)
        {
            PC += 4;
            wp_decode_2_execute->write( instr, cycle);
            wp_decode_2_fetch_stall->write( false, cycle);
        } else {
            wp_decode_2_fetch_stall->write( true, cycle);
        }
    }
}

void PerfMIPS::clock_execute( uint64 cycle)
{
    bool is_stall;
    rp_memory_2_execute_stall->read(&is_stall, cycle);
    if ( is_stall)
    {
        if ( !silent_mode) cout << "execute cycle " << cycle <<":  " << "stall" << endl;
        wp_execute_2_decode_stall->write( true, cycle);
        return;
    }
    FuncInstr instr;
    if ( !rp_decode_2_execute->read( &instr, cycle))
    {
        if ( !silent_mode) cout << "execute cycle " << cycle <<":  " << "stall" << endl;
        return;
    }
    instr.execute();
    if ( !silent_mode) cout << "execute cycle " << cycle <<":  " << instr << endl;
    wp_execute_2_memory->write( instr, cycle);
    wp_execute_2_decode_stall->write( false, cycle);
}


void PerfMIPS::clock_memory( uint64 cycle)
{
    bool is_stall;
    rp_wback_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        if ( !silent_mode) cout << "memory  cycle " << cycle <<":  " << "stall" << endl;
        wp_memory_2_execute_stall->write( true, cycle);
        return;
    }
    FuncInstr instr;
    if ( !rp_execute_2_memory->read( &instr, cycle))
    {
        if ( !silent_mode) cout << "memory  cycle " << cycle <<":  " << "stall" << endl;
        return;
    }
    load_store( instr);
    if ( !silent_mode) cout << "memory  cycle " << cycle <<":  " << instr << endl;
    wp_memory_2_wback->write( instr, cycle);
    wp_memory_2_execute_stall->write( false, cycle);
}

void PerfMIPS::clock_wback( uint64 cycle)
{
    FuncInstr instr;
    if ( !rp_memory_2_wback->read( &instr, cycle))
    {
        if ( !silent_mode) cout << "wback   cycle " << cycle <<":  " << "stall" << endl;
        return;
    }
    wb( instr);
    PC = instr.get_new_PC();
    PC_is_valid = true;
    executed_instr++;
    if ( !silent_mode) cout << "wback   cycle " << cycle <<":  ";
    cout << instr << endl;
    wp_wback_2_memory_stall->write( false, cycle);
}

void PerfMIPS::run(const std::string& tr, uint32 instrs_to_run, bool is_silent)
{
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    executed_instr = 0;
    silent_mode = is_silent;
    while ( executed_instr < instrs_to_run)
    {
        clock_fetch(cycle);
        clock_decode(cycle);
        clock_execute(cycle);
        clock_memory(cycle);
        clock_wback(cycle);
        ++cycle;
    }
}


PerfMIPS::~PerfMIPS()
{
    delete rf;

    delete rp_fetch_2_decode;
    delete wp_fetch_2_decode;
    delete rp_decode_2_execute;
    delete wp_decode_2_execute;
    delete rp_execute_2_memory;
    delete wp_execute_2_memory;
    delete rp_memory_2_wback;
    delete wp_memory_2_wback;
    
    delete rp_wback_2_memory_stall;
    delete wp_wback_2_memory_stall;
    delete rp_memory_2_execute_stall;
    delete wp_memory_2_execute_stall;
    delete rp_execute_2_decode_stall;
    delete wp_execute_2_decode_stall;
    delete rp_decode_2_fetch_stall;
    delete wp_decode_2_fetch_stall;


}