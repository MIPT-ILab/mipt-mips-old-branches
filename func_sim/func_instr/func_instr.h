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
            cerr << "Fatal error: "     \
                 << message << ", "      \
                 << endl;                 \
            abort();                       \
        }
#else
    # define ASSERT( cond, messege )
#endif

class InstrList
{
public:
    // TYPES ***************************//
    enum REG                            //
    {                                   //
        ZERO,                           //
        AT,                             //
        V0, V1,                         //
        A0, A1, A3,                     //
        T0, T1, T2, T3, T4, T5, T6, T7, //
        S0, S1, S2, S3, S4, S5, S6, S7, //
        T8, T9,                         //
        K0, K1,                         //
        GP,                             //
        SP,                             //
        FP,                             //
        RA,                             //
    };                                  //
                                        // 
    enum REGTYPE { S, T, D };           //
                                        //
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
     } typedef Parsed;                  //
     //*********************************//

	// INNER CLASS ************************************//
    class FuncInstr                                    //
    {                                                  //
    public:                                            //
        friend class InstrList;                        //
                                                       //
        // INTERFACE ******************************//  //
        FuncInstr(uint32 bytes);                   //  //
       ~FuncInstr() {};                            //  //
        void initFormat(uint32 bytes);             //  //
        void parseR    (uint32 bytes);             //  //
        void parseI    (uint32 bytes);             //  //
        void parseJ    (uint32 bytes);             //  //
        const char *get_name(REGTYPE type) const;  //  //
        string Dump(string indent = " ") const;    //  //
        //*****************************************//  //
    private:                                           //
        // DATA ***********************************//  //
        Parsed instr;                              //  //
        FORMAT format;                             //  //
        TYPE type;                                 //  //
        const char *name;                          //  //
        //*****************************************//  //
    };                                                 //
    //*************************************************//

    // INTERFACE ***********************************//
    InstrList(const char *name):                    //
        section_name(name) {}                       //
    InstrList() {}                                  //
    inline void add(uint32 bytes)                   //  
    {                                               //
        FuncInstr entry(bytes);                     // 
        isaTable.push_back(entry);                  //
    }                                               //
    string Dump(string indent = " ") const;         //
    // *********************************************//
private:
    // DATA ****************************************//
    const char *section_name;                       //
    vector<FuncInstr> isaTable;                     //
    // *********************************************//
};

inline ostream &operator<<(ostream &out, const InstrList &instr)
{
    out << instr.Dump("");
    return out;
}

inline ostream &operator<<(ostream &out, const FuncInstr &instr)
{
    out << instr.Dump("");
    return out;
}

uint32 get_instr(uint8 *where);
