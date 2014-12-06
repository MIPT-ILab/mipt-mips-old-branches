/***************************************************************

    The interface for instruction decoder

    @date: December 3, 2014
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @version: 1.0 (December 3, 2014) 

***************************************************************/

// Generic C++
#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <func_memory.h>

using namespace std;

//#define NDEBUG

#ifndef NDEBUG
    #define ASSERT( cond, message ) \
        if ( !( cond ) )             \
                                      \
        {                              \
            cerr << message             \
                 << endl;                \
            exit(EXIT_FAILURE);           \
        }
#else
    # define ASSERT( cond, messege )
#endif

class FuncInstr                                    
{                                                  
public:                                            
    friend class InstrList;                        
    // TYPES ***************************//
    enum FORMAT { R, I, J, UNEXP };     //
                                        //
    enum TYPE                           //
    {                                   //
        ADD,                            //
        SUB,                            //
        SHIFT,                          //
        BRNCH,                          //
        JUMP                            //
    };                                  //
                                        //
    union Parsed                        //
    {                                   //
        struct                          //
        {                               //
            unsigned imm:16;            //
            unsigned t:5;               //
            unsigned s:5;               //
            unsigned opcode:6;          //
        } I;                            //
        struct                          //
        {                               //
            unsigned funct:6;           //
            unsigned shamt:5;           //
            unsigned d:5;               //
            unsigned t:5;               //
            unsigned s:5;               //
            unsigned opcode:6;          //
        } R;                            //
        struct                          //
        {                               //
            unsigned addr:26;           //
            unsigned opcode:6;          //
        } J;                            //
        uint32 raw;                     //
    } typedef Parsed;                   //
                                        //
    struct ISAEntry                     //
    {                                   //
        const char *name;               //
        uint8 opcode;                   //
        uint8 funct;                    //
        FORMAT format;                  //
        TYPE type;                      //
        uint8 reg_num;                  //
        uint8 const_num;                //
    }                                   //
    //**********************************//

    // INTERFACE ******************************//  
    FuncInstr(uint32 bytes);                   //  
   ~FuncInstr() {};                            //  
    void initFormat(uint32 bytes);             //  
    void parseR    (uint32 bytes);             //  
    void parseI    (uint32 bytes);             //  
    void parseJ    (uint32 bytes);             //    
    string Dump(string indent = " ") const;    //  
    //*****************************************//  
private:                                           
    // DATA ***********************************//  
    Parsed instr;                              //
                                               //
    static const ISAEntry[] isaTable;          //
    static const char[][] reg_names;           //  
                                               //
    ISAEntry instr_feat;                       //
    vector<const char *> regist;               //
    uint32 const_val;                          //  
    //*****************************************//  
};                                                 

inline ostream &operator<<(ostream &out, const FuncInstr &instr)
{
    out << instr.Dump("");
    return out;
}

class InstrList
{
public:
    // INTERFACE ***********************************//
    InstrList(const char *name):                    //
        section_name(name) {}                       //
   ~InstrList() {}                                  //
    inline void add(uint32 bytes)                   //  
    {                                               //
        FuncInstr entry(bytes);                     // 
        list.push_back(entry);                      //
    }                                               //
    string Dump(string indent = " ") const;         //
    // *********************************************//
//private:
    // DATA ****************************************//
    const char *section_name;                       //
    vector<FuncInstr> list;                         //
    // *********************************************//
};

inline ostream &operator<<(ostream &out, const InstrList &instr)
{
    out << instr.Dump("");
    return out;
}

// Functions for disassembler:
uint32 get_instr(uint8 *where);
void print_sec(vector<FuncInstr>::iterator it);
