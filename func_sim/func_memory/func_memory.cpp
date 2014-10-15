/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <math.h>
#include <assert.h>

// Generic C++
#include <iostream>
#include <vector>
#include <sstream>


// uArchSim modules
#include "func_memory.h"
#include "../elf_parser/elf_parser.h"

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits):
        ArraySet(1 << (addr_size - page_bits - offset_bits)),
        _addr_size(addr_size), _page_bits(page_bits), _offset_bits(offset_bits)
{
    cout << executable_file_name << endl;
    //vector<uint64> numSet = getAddr(0x405b150, addr_size, page_bits, offset_bits);
    //cout << hex << numSet[0] << ' ' << numSet[1] << ' ' << numSet[2] << dec << endl;
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections(executable_file_name, sections_array);
    cout << ArraySet.size() << endl;
    for(int i = 0; i < sections_array.size(); i++) {
        cout << sections_array[i].dump();
        for(uint64 j = 0; j <= sections_array[i].size; j ++) {
            vector<uint64> numSet = getAddr(sections_array[ i].start_addr + j,
                    addr_size,
                    page_bits,
                    offset_bits);
            assert(numSet[0] < ArraySet.size());
            checkAddrAndAdd(numSet);
            //assert((2 * j) < sections_array[i].size);
            //cout << hex << sections_array[i].start_addr + j << ":" << (uint16)*((uint8*)(sections_array[i].content) + j) << dec << endl;
            //uint32 value = (uint16)*((uint8*)(sections_array[i].content) + j);
            ArraySet.at(numSet[0])->
                     at(numSet[1])->
                     at(numSet[2]) = (uint16)*((uint8 *)sections_array[i].content + j);
            //cout << "VAL:" << ArraySet.at(numSet[0])->at(numSet[1])->at(numSet[2]) << endl;
        }
    }
}

FuncMemory::~FuncMemory()
{
    // put your code here
    //cout << "Destruct" << endl;
    for(uint64 i = 0; i < ArraySet.size(); i++) {
        assert(i < ArraySet.size());
        if(ArraySet.at(i) != NULL) {
            for (uint64 j = 0; j < ArraySet.at(i)->size(); j++) {
                assert(j < ArraySet.at(i)->size());
                delete ArraySet.at(i)->at(j);
            }
        }
        delete ArraySet.at(i);
    }
}

uint64 FuncMemory::startPC() const
{
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here
    uint64 value = 0;
    for(unsigned short i = 0; i < num_of_bytes; i++) {
        cout << value << endl;
        vector<uint64> myAddr = getAddr(addr + num_of_bytes - i - 1, _addr_size, _page_bits, _offset_bits);
        if(checkAddr(myAddr)) {
            value = (value << (BITS_IN_BYTE * sizeof(uint8))) + ArraySet.at(myAddr[0])->at(myAddr[1])->at(myAddr[2]);
        }
        else {
            value = value << (BITS_IN_BYTE * sizeof(uint8));
        }
    }
    return value;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes) {
    // put your code here
    for (unsigned short i = 0; i < num_of_bytes; i++) {
        cout << value << endl;
        cout << "k=" << (1 << (sizeof(uint8) * BITS_IN_BYTE)) << endl;
        uint8 byte = value % (1 << (sizeof(uint8) * BITS_IN_BYTE));
        cout << "byte:" << hex << byte << dec << endl;
        value = value >> (sizeof(uint8) * BITS_IN_BYTE);
        vector <uint64> myAddr = getAddr(addr + i);
        checkAddrAndAdd(myAddr);
        assert(ArraySet.at(myAddr.at(0)) != NULL);
        assert(ArraySet.at(myAddr.at(0))->at(myAddr.at(1)) != NULL);
        ArraySet.at(myAddr.at(0))->at(myAddr.at(1))->at(myAddr.at(2)) = byte;
    }
}
string FuncMemory::dump( string indent) const
{
    // put your code here
    ostringstream oss;
    bool is_skipSet = 0;
    for(uint64 i = 0; i < ArraySet.size(); i++) {
        if(ArraySet.at(i) != NULL) {
            is_skipSet = 0;
            bool is_skipPage = 0;
            oss << "SET: " << hex << ArraySet[i] << dec << endl;
            for(uint64 j = 0; j < ArraySet.at(i)->size(); j++) {
                if(ArraySet.at(i)->at(j) != NULL) {
                    is_skipPage = 0;
                    bool is_skipVal = 0;
                    oss << "  PAGE: "  << hex << ArraySet.at(i)->at(j) << dec << endl;
                    for(uint64 k = 0; k < ArraySet.at(i)->at(j)->size(); k++) {
                        if(ArraySet.at(i)->at(j)->at(k) != 0) {
                            is_skipVal = 0;
                            oss << "    0x" << hex << ((i << (_page_bits + _offset_bits)) + (j << (_offset_bits)) + k) << dec
                                    << ":" <<"0x" << hex << ArraySet.at(i)->at(j)->at(k) << dec << endl;
                        }
                        else {
                            if(!is_skipVal) {
                                oss << "    ...." << endl;
                                is_skipVal = 1;
                            }
                        }
                    }
                }
                else {
                    if(!is_skipPage) {
                        oss << "PAGE...." << endl;
                        is_skipPage = 1;
                    }
                }
            }
        }
        else {
            if(!is_skipSet) {
                oss << "SET...." << endl;
                is_skipSet = 1;
            }
        }
    }
    return oss.str();
    //return string("ERROR: You need to implement FuncMemory!");
}

uint64 FuncMemory::getMask(uint64 num_digit) const{
    uint64 mask = 0;
    for(uint64 i = 0; i < num_digit; i++) {
        mask = (mask << 1) + 1;
    }
    return mask;
}

vector<uint64> FuncMemory::getAddr( uint64 full_addr,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits) const
{
    vector<uint64> addr(3);
    addr[0] = (full_addr >> (page_bits + offset_bits)) & getMask(addr_size - page_bits - offset_bits);
    addr[1] = (full_addr >> (offset_bits))             & getMask(page_bits);
    addr[2] =  full_addr                               & getMask(offset_bits);
    return addr;
}

vector<uint64> FuncMemory::getAddr( uint64 full_addr) const {
    vector<uint64> addr(3);
    addr[0] = (full_addr >> (_page_bits + _offset_bits)) & getMask(_addr_size - _page_bits - _offset_bits);
    addr[1] = (full_addr >> (_offset_bits))             & getMask(_page_bits);
    addr[2] =  full_addr                               & getMask(_offset_bits);
    return addr;
}

bool FuncMemory::checkAddrAndAdd(const vector<uint64> &addr) {
    //assert(addr.size() != 3);
    if(ArraySet.at(addr[0]) == NULL) {
        //cout << "Init SET" << numSet[0] << endl;
        ArraySet.at(addr[0]) = new Page(1 << _page_bits);
        assert(ArraySet[addr[0]] != NULL);
    }
    if(ArraySet.at(addr[0])->at(addr[1]) == NULL) {
        //cout << "Init PAGE" << numSet[1] << endl;
        ArraySet.at(addr[0])->at(addr[1]) = new Cell(1 << _offset_bits);
        assert(ArraySet.at(addr[0])->at(addr[1]) != NULL);
    }
    return true;
}

bool FuncMemory::checkAddr(const vector<uint64> &addr) const {
    if(ArraySet[addr[0]] == NULL) {
        return false;
    }
    if(ArraySet.at(addr[0])->at(addr[1]) == NULL) {
        return false;
    }
    return true;
}