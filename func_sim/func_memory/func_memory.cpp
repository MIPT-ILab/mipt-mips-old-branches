/**
* func_memory.cpp - the module implementing the concept of
* programer-visible memory space accesing via memory address.
* @author Alexander Titov <alexander.igorevich.titov@gmail.com>
* Copyright 2012 uArchSim iLab project
*/

/*******************************************************

	Reducted by:
 
    @date: October 13, 2014
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @vertion: 1.0 (October 13, 2014) 

*******************************************************/

// Generic C
#include <cstring>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>
#include <bitset>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

using namespace std;

uint64 SetBytes(const uint8 num, const uint8 length)
{
        assert(num + length <= 64);

        uint64 res = 0;
        for (int i = 0; i < num; i++)
                res |= 1 << (length + i);
        
        return res;
}

FuncMemory::FuncMemory(const char* executable_file_name,
                       uint64 addr_size,
                       uint64 page_num_size,
                       uint64 offset_size):
_addr_bits   (addr_size),
_set_bits    (addr_size - page_num_size - offset_size),
_page_bits   (page_num_size),
_offset_bits (offset_size),
_file_name   ((char*) executable_file_name)
{
        assert(executable_file_name != NULL);
        assert(addr_size <= 64);
        assert(page_num_size < 64);
        assert(offset_size < 64);

        vector<ElfSection> sections;
        ElfSection::getAllElfSections(executable_file_name, sections);
        assert(sections.size() != 0);

        _set_mask = SetBytes(_set_bits, _page_bits + _offset_bits);
        _page_mask = SetBytes(_page_bits, _offset_bits);
        _offset_mask = SetBytes(_offset_bits);

        _begin_addr = sections[0].start_addr;
        _end_addr = sections[sections.size() - 1].start_addr +
                    sections[sections.size() - 1].size - 1;

        for (int i = 0; i < sections.size(); i++) {
                if (!strcmp(sections[i].name,".text"))
                        _text_start = sections[i].start_addr;

                for (int j = 0; j < sections[i].size; j++)
                        write(sections[i].content[j], sections[i].start_addr + j, 1);
        }
}

const uint8* FuncMemory::RSearch(const uint64 addr) const
{
        assert(addr != 0);

        uint64 set_addr = (addr & _set_mask) >> (_page_bits + _offset_bits);
        uint64 page_addr = (addr & _page_mask) >> _offset_bits;
        uint64 offset_addr = addr & _offset_mask;

        assert(_memory.size() > set_addr);
        assert(_memory[set_addr].size() > page_addr);

        for (int i = 0; i < _memory[set_addr][page_addr].size(); i++) {
                if (_memory[set_addr][page_addr][i].addr == offset_addr)
                        return &_memory[set_addr][page_addr][i].value;
        }

        return NULL;
}

uint8* FuncMemory::WSearch(const uint64 addr)
{
        assert(addr != 0);

        uint64 set_addr = (addr & _set_mask) >> (_page_bits + _offset_bits);
        uint64 page_addr = (addr & _page_mask) >> _offset_bits;
        uint64 offset_addr = addr & _offset_mask;

        if (_memory.size() <= set_addr)              
                _memory.resize(set_addr + 1);
        if (_memory[set_addr].size() <= page_addr)
                _memory[set_addr].resize(page_addr + 1);

        for (int i = 0; i < _memory[set_addr][page_addr].size(); i++) {
                if (_memory[set_addr][page_addr][i].addr == offset_addr)
                        return &_memory[set_addr][page_addr][i].value;
        }

        Pair_t new_p;
        new_p.addr = offset_addr;
        new_p.value = 0;
        _memory[set_addr][page_addr].push_back(new_p);
        return &_memory[set_addr][page_addr].back().value;
}

uint64 FuncMemory::read(uint64 addr, unsigned short num_of_bytes) const
{
        assert(num_of_bytes <= 8);
        assert(addr != 0);

        uint64 res = 0;
        for (int i = 0; i < num_of_bytes; i++) {
                res <<= 8;
                res += *RSearch(addr + i);
        }

        return res;
}

void FuncMemory::write(uint64 value, uint64 addr, unsigned short num_of_bytes)
{
        assert(num_of_bytes <= 8);
        assert(addr != 0);

        uint64 mask = SetBytes(8);
        
        if (value == 0)
                return;
        for (int i = num_of_bytes; i > 0; i--) {
                *WSearch(addr + i - 1) = (value & mask) >> ((num_of_bytes - i) * 8);
                mask <<= 8;
        }
}

string FuncMemory::dump(string indent) const
{
        ostringstream oss;

        oss << indent << "Functional Memory Dump" << endl
            << indent << "Executable file name: " << _file_name << endl
            << indent << "First address: 0x" << hex << _begin_addr << dec << endl
            << indent << "Last address: 0x" << hex << _end_addr << dec << endl
            << indent << "Content:" << endl
            << indent << "|SET       |PAGE      |OFFSET:  VALUE     |" << endl;

        indent.push_back(' ');
        for (int i = 0; i < _memory.size(); i++) {
                if (_memory[i].size()) {
                        oss << indent << "0x" << hex << i << dec << endl;
                        string set_indent = indent;
                        set_indent.push_back(' ');
                        oss << SetDump(_memory[i], set_indent);
                }
        }

    return oss.str();
}

string FuncMemory::SetDump(const vector< vector<Pair_t> > set_addr, const string indent) const
{
        ostringstream oss;

        uint64 i = 0;
        while (!set_addr[i].size() && i < set_addr.size()) i++;

        char sym = '\0';
        while (i < set_addr.size()) {
                uint64 curr_page = i;
                i++;
                while (!set_addr[i].size() && i < set_addr.size()) i++;

                if (i == set_addr.size())
                        sym = ' ';
                else
                        sym = '|';

                oss << indent << sym << "`--------0x" << hex << curr_page << dec << endl;
                
                string page_indent = indent;
                page_indent.push_back(sym);
                page_indent.append("          ");
                oss << PageDump(set_addr[curr_page], page_indent);
        }

        oss << endl;
        return oss.str();
}

string FuncMemory::PageDump(const vector<Pair_t> page_addr, const string indent) const
{
        ostringstream oss;

        char sym = '|';
        for (int i = 0; i < page_addr.size(); i++) {
                if (i == page_addr.size() - 1)
                sym = ' ';
                
                oss << indent << sym << "`--------0x" << hex << page_addr[i].addr << ":  "
                    << bitset<8>(page_addr[i].value) << endl;
        }

        oss << indent << endl;
        return oss.str();
}
