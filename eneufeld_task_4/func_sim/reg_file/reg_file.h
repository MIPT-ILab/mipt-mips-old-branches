#ifndef REG_FILE
#define REG_FILE

#include <string>

#include <types.h>



enum RegName
{
    zero,
    at,
    v0,
    v1,
    a0, a1, a2, a3,
    t0, t1, t2, t3, t4, t5, t6, t7,
    s0, s1, s2, s3, s4, s5, s6, s7,
    t8, t9,
    k0, k1,
    gp,
    sp,
    fp,
    ra,
    RegCount
};

class RegFile 
{
private:
    sint64 reg_array[ RegCount];
    static const char *reg_name[];
public:
    RegFile();
    sint64 read( int number);
    void write( sint64 reg_data, int number);
    std::string dump( std::string indent = "") const;
    std::string dump( int number) const;
};
#endif //REG_FILE
