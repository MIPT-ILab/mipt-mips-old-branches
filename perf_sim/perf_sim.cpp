/* 
 * perf_sim.cpp - scalar MIPS simulator
 * @author Semyon Abramov semyon.abramov.mipt@gmail.com 
 * MIPT-MIPS 2015
 */

#include"perf_sim.h"

using namespace std;

PerfMIPS::PerfMIPS()
{
	
	wp_fetch_to_decode = new WritePort< uint32>( "FETCH_TO_DECODE", PORT_BW, PORT_FANOUT);
	rp_fetch_to_decode = new ReadPort< uint32>( "FETCH_TO_DECODE", PORT_LATENCY);
	wp_decode_to_execute = new WritePort< FuncInstr>( "DECODE_TO_EXECUTE", PORT_BW, PORT_FANOUT);
	rp_decode_to_execute = new ReadPort< FuncInstr>( "DECODE_TO_EXECUTE",  PORT_LATENCY);
	wp_execute_to_memory = new WritePort< FuncInstr>( "EXECUTE_TO_MEMORY", PORT_BW, PORT_FANOUT);
	rp_execute_to_memory = new ReadPort< FuncInstr>( "EXECUTE_TO_MEMORY",  PORT_LATENCY);
	wp_memory_to_writeback = new WritePort< FuncInstr>( "MEMORY_TO_WRITEBACK", PORT_BW, PORT_FANOUT);
	rp_memory_to_writeback = new ReadPort< FuncInstr>( "MEMORY_TO_WRITEBACK",  PORT_LATENCY);
	
	wp_decode_to_fetch_stall = new WritePort< bool>( "DECODE_TO_FETCH_STALL", PORT_BW, PORT_FANOUT);
	rp_decode_to_fetch_stall = new ReadPort< bool>( "DECODE_TO_FETCH_STALL",  PORT_LATENCY);
	wp_execute_to_decode_stall = new WritePort< bool>( "EXECUTE_TO_DECODE_STALL", PORT_BW, PORT_FANOUT);
	rp_execute_to_decode_stall = new ReadPort< bool>( "EXECUTE_TO_DECODE_STALL",  PORT_LATENCY);
	wp_memory_to_execute_stall = new WritePort< bool>( "MEMORY_TO_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
	rp_memory_to_execute_stall = new ReadPort< bool>( "MEMORY_TO_EXECUTE_STALL",  PORT_LATENCY);
	wp_writeback_to_memory_stall = new WritePort< bool>( "WRITEBACK_TO_MEMORY_STALL", PORT_BW, PORT_FANOUT);
	rp_writeback_to_memory_stall = new ReadPort< bool>( "WRITEBACK_TO_MEMORY_STALL",  PORT_LATENCY);
	
	decode_instr = new FuncInstr( 0, 0);
	exec_instr = new FuncInstr( 0, 0);
	mem_instr = new FuncInstr( 0, 0);
	
	ready = true; 
	rf = new RF();
	PC = new Pc();

}


PerfMIPS::~PerfMIPS() 
{
	delete rf;       
	delete PC; 
	delete rp_fetch_to_decode;
	delete wp_fetch_to_decode;
	delete rp_decode_to_execute;
	delete wp_decode_to_execute;
	delete rp_execute_to_memory;
	delete wp_execute_to_memory;
	delete rp_memory_to_writeback;
	delete wp_memory_to_writeback;
	delete rp_decode_to_fetch_stall;
	delete wp_decode_to_fetch_stall;
	delete rp_execute_to_decode_stall;
	delete wp_execute_to_decode_stall;
	delete rp_memory_to_execute_stall;
	delete wp_memory_to_execute_stall;
	delete rp_writeback_to_memory_stall;
	delete wp_writeback_to_memory_stall;
	delete decode_instr;
	delete exec_instr;
	delete mem_instr;
}

void PerfMIPS::read_src( FuncInstr& instr)  
{
	rf->read_src1( instr); 
	rf->read_src2( instr);
			
	if ( instr.isJump()) // check on jumps and branches
	{
		PC->is_valid_in_fetch = false;
		PC->is_valid_in_decode = false;
	}
			
	RegNum reg_num = instr.get_dst_num();
	RegNum reg_num1 = instr.get_src1_num();
	RegNum reg_num2  = instr.get_src2_num();
			
	if ( ( ( reg_num1 == reg_num) || ( reg_num2 == reg_num)) 
		&& ( ( rf->checkValid( reg_num1)) && ( rf->checkValid( reg_num2))))
	{
		rf->makeSame( reg_num); // set marker that source and producer refer to the same register
		rf->invalidate( reg_num); // invalidate reg till wb
	}
	else
	{	
		rf->invalidate( reg_num);
	}
	return;
}

bool PerfMIPS::src_ready( FuncInstr& instr) 
{
	RegNum reg_num1 = instr.get_src1_num();
	RegNum reg_num2 = instr.get_src2_num();
			
	if ( ( ( rf->checkValid( reg_num1)) && ( rf->checkValid( reg_num2)))
		|| ( rf->checkSame( reg_num1)) || ( rf->checkSame( reg_num2)))
	{
		rf->makeDifferent( reg_num1); // remove marker 
		rf->makeDifferent( reg_num2); // remove marker
		return true;
	}
	else
	{
		return false;
	}
}

void PerfMIPS::clock_fetch( int cycle)
{
	bool is_stall = false;
	rp_decode_to_fetch_stall->read( &is_stall, cycle);
	
	if ( is_stall)
	{
		if ( !silent_output)
		{	
			cout << "fetch\t" << "cycle " << cycle << ":\t" << "0x" << std :: hex << bytes << std::dec << endl;
		}
		
		wp_fetch_to_decode->write( this->bytes, cycle);
		return;
	}
	else
	{
		if ( !PC->is_valid_in_fetch)
		{
			if ( !silent_output)
			{	
				cout << "fetch\t" << "cycle " << cycle << ":\t" << "0x" << std :: hex << bytes << std::dec << " ( PC is not valid)" << endl;
			}
			return;
		}
		
		this->bytes = mem->read( PC->value, 4);
		
		if ( !silent_output)
		{
			cout << "fetch\t" << "cycle " << cycle << ":\t" << "0x" << std :: hex << bytes << std::dec << endl;
		}
		
		wp_fetch_to_decode->write( this->bytes, cycle);
		return;
	}
}

void PerfMIPS::clock_decode( int cycle)
{
	bool is_stall = false;
	rp_execute_to_decode_stall->read( &is_stall, cycle);
	
	if ( is_stall)
	{
		wp_decode_to_fetch_stall->write( true, cycle);
		return;
	}
	
	uint32 data = 0x0;
	rp_fetch_to_decode->read( &data, cycle);
	
	if ( cycle != 0)
	{	
		/* if get ready, store new instruction to internal data of latch */ 
		if ( ready)
		{
			decode_instr = new FuncInstr( data, PC->value); 
		}
			
		if ( !PC->is_valid_in_decode)
		{
			if ( !silent_output)
			{	
				cout << "decode\t" << "cycle " << cycle << ":\t" << "bubble" << endl;
			}
			return;
		}
			
		read_src( *decode_instr);
			
		if ( src_ready( *decode_instr))
		{
			if ( !silent_output)
			{	
				cout << "decode\t" << "cycle " << cycle << ":\t" << *decode_instr << endl;
			}
			
			wp_decode_to_execute->write( *decode_instr, cycle);
			wp_decode_to_fetch_stall->write( false, cycle);
			PC->value = PC->value + 4;
		    ready = true;
		}
		else
		{
			ready = false;
			
			if ( !silent_output)
			{	
				cout << "decode\t" << "cycle " << cycle << ":\t" << *decode_instr << endl;
				cout.width( 29);
				cout << "STALL" << endl;
			}
			
			wp_decode_to_fetch_stall->write( true, cycle);
		}
	}
	else
	{
		if (!silent_output)
		{	
			cout << "decode\t" << "cycle " << cycle << ":\t" << "bubble"<< endl;
		}
		PC->value = PC->value + 4;
	}
	return;
}


void PerfMIPS::clock_execute( int cycle)
{
	bool is_stall = false;
	rp_memory_to_execute_stall->read( &is_stall, cycle);
	
	if ( is_stall)
	{
		wp_execute_to_decode_stall->write( true, cycle);
		return;
	}
	else
	{
		wp_execute_to_decode_stall->write( false, cycle);
	}
	
	FuncInstr instr;
	
	if ( rp_decode_to_execute->read( &instr, cycle))
	{
		
		*exec_instr = instr;
		
		if ( !silent_output)
		{	
			cout << "execute\t" << "cycle " << cycle << ":\t" << *exec_instr << endl;
		}
		
		exec_instr->execute();
		
		/* if executing instruction is jump 
		it must update PC */
		if ( exec_instr->isJump())
		{
			PC->is_valid_in_fetch = true;
			PC->value = exec_instr->get_new_PC();
		}
		
		wp_execute_to_memory->write( *exec_instr, cycle);
	}
	else
	{
		if (!silent_output)
		{
			cout << "execute\t" << "cycle " << cycle << ":\t" << "bubble" << endl;
		}	
	}
	
	return;
}

void PerfMIPS::clock_memory( int cycle)
{
		
	FuncInstr instr;
		
	if ( rp_execute_to_memory->read( &instr, cycle))
	{
		*mem_instr = instr; 
			
		/* if instruction in memory is jump/branch it must set 
		valid bit in decode and promote PC by 4 to fetch next instruction 
		because previous right instruction has been already written to the port 
		between fetch and decode */
		if ( mem_instr->isJump())
		{
			PC->is_valid_in_decode = true;
			PC->value = PC->value + 4;
		}
			
		load_store( *mem_instr);
		
		if( !silent_output)
		{	
			memory_moduleDump( *mem_instr);
		}
		
		wp_memory_to_writeback->write( *mem_instr, cycle);
	}
	else
	{
		if( !silent_output)
		{
			cout << "memory\t" << "cycle " << cycle << ":\t" << "bubble" << endl;
		}
	}
	
	return;	
}

void PerfMIPS::clock_writeback( int cycle)
{
	FuncInstr exec_instr;
	
	if ( rp_memory_to_writeback->read( &exec_instr, cycle))
	{	
		rf->write_dst( exec_instr);
		
		if ( silent_output)
		{
			cout << exec_instr << endl;
		}
		else
		{
			cout << "wb\t" << "cycle " << cycle << ":\t" << exec_instr << endl;
		}
		
		++executed_instr;
	}	
	else if ( !silent_output)
	{
		cout << "wb\t" << "cycle " << cycle << ":\t" << "bubble" << endl;
	}
}

void PerfMIPS::run( const std::string& tr, int instrs_to_run, bool silent)
{
	mem = new FuncMemory( tr.c_str());
    PC->value = mem->startPC();
	silent_output = silent;
	executed_instr = 0;
	cycle = 0;  
	
	while( executed_instr < instrs_to_run)
	{
		clock_fetch( cycle);
		clock_decode( cycle);
		clock_execute( cycle);
		clock_memory( cycle);
		clock_writeback( cycle);
		++cycle;
		
		if ( !silent_output) { cout << "/*******************/" << endl;}
	}
	
	delete mem;
}



	



