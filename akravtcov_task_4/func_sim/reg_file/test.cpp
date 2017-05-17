#include <cassert>

#include <iostream>

#include <types.h>
#include <reg_file.h>

int main()
{
    RegFile reg;

    uint64 addr = 0x12345678;

    reg.write( addr, RegFile::T0);
    assert( reg.read( RegFile::T0) == addr);

    assert( reg.read( RegFile::S0) == NO_VAL64);

    std::cout << "finished successfully" << std::endl;
    return 0;
}
