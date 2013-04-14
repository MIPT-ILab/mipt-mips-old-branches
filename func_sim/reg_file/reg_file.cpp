/*

*/

// Generic C
#include <assert.h>
#include <string.h>

// uArchSim modules
#include <reg_file.h>
#include <types.h>

RegFile::RegFile( size_t size)
	: size(size)
{
    regfile = new uint64[ size];
    memset( regfile, 0, sizeof( uint64) * size);
}

RegFile::~RegFile()
{
  delete [] regfile;
}

uint64 RegFile::read( size_t register_id)
{
	assert( register_id < size);
	return regfile[ register_id];
}

void RegFile::write( size_t register_id, uint64 val)
{
	assert( register_id < size);
    regfile[ register_id] = val;
}
