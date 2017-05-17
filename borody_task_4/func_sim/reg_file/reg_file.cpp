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
    regfile = new sint64[ size];
    memset( regfile, 0, sizeof( sint64) * size);
}

RegFile::~RegFile()
{
  delete [] regfile;
}

sint64 RegFile::read( size_t register_id) const
{
	assert( register_id < size);
	return regfile[ register_id];
}

void RegFile::write( size_t register_id, sint64 val)
{
	assert( register_id < size);
    regfile[ register_id] = val;
}
