#ifndef INSTRUCTIONS
#define INSTRUCTIONS

enum InstructionType
{
    R_TYPE = 0x00,
    I_TYPE = 0x2B,
    J_TYPE = 0x03
};

enum RtypeInstruction
{
    ADD  = 0x20,
    ADDU = 0x21,
    SUB  = 0x22,
    SUBU = 0x23,
    SLL  = 0x00,
    SRL  = 0x02,
    JR   = 0x08
};

enum ItypeInstruction
{
    ADDI  = 0x08,
    ADDIU = 0x09,
    BEQ   = 0x04,
    BNE   = 0x05,
    LW    = 0x23,
    SW    = 0x2b,
    LUI   = 0x0f
};

enum JtypeInstruction
{
    J = 0x02
};

#endif //INSTRUCTIONS
