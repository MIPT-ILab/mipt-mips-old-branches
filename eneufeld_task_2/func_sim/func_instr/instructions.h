#ifndef INSTRUCTIONS
#define INSTRUCTIONS

enum InstructionType
{
    Rtype = 0x00,
    Itype = 0x2B,
    Jtype = 0x03
};

enum RtypeInstruction
{
    add  = 0x20,
    addu = 0x21,
    sub  = 0x22,
    subu = 0x23,
    sll  = 0x00,
    srl  = 0x02,
    jr   = 0x08
};

enum ItypeInstruction
{
    addi  = 0x08,
    addiu = 0x09,
    beq   = 0x04,
    bne   = 0x05,
};

enum JtypeInstruction
{
    j = 0x02
};

#endif //INSTRUCTIONS
