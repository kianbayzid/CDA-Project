#include "spimcore.h"


// Takes in A, B & ALUControl to produce ALUresult which sets the value of Zero
/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    //ALUResult based on control and what function it sets

    //Switch to run the operation based on the control
   switch (ALUControl) {
    case 0: // 000: A + B
        *ALUresult = A + B;
        break;
    case 1: // 001: A - B
        *ALUresult = A - B;
        break;
    case 2: // 010 (signed A < signed B)
        *ALUresult = ((int)A < (int)B) ? 1 : 0;
        break;
    case 3: //
        *ALUresult = (A < B) ? 1 : 0;
        break;
    case 4: //
        *ALUresult = A & B;
        break;
    case 5: //
        *ALUresult = A | B;
        break;
    case 6: //
        *ALUresult = B << 16;
        break;
    case 7: //
        *ALUresult = ~A;
        break;
    default:
        *ALUresult = 0;
        break;
   }
   // Zero flag
   *Zero = (*ALUresult == 0) ? 1 : 0;
}

// Takes PC & Mem array to check word-alignment, memory range verification, 
// where word is loaded properly, halt if illegal
/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    //What is PC
}

//Uses 32-bit instructions to extract fields
/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    /*
    Use shifts & masks
    Bits - meanings
        31-26	op (6 bits)
        25-21	r1 (rs) (5 bits)
        20-16	r2 (rt) (5 bits)
        15-11	r3 (rd) (5 bits)
        5-0	    funct   (6 bits)
        15-0	offset  (16 bits)
        25-0	jsec    (26 bits)
    */
    op = instruction >> 26;
    r1 = (instruction >> 21) & 0x1F;
    r2 = (instruction >> 16) & 0x1F;
    r3 = (instruction >> 11) & 0x1F;
    funct = instruction & 0x3F;   // 0x3F = 6 bits of 1’s
    offset = instruction & 0xFFFF;   // 16 bits
    jsec = instruction & 0x3FFFFFF;
}


//Sets control signals
/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // Use the instruction list in PDF Appendix A.
    // Read op and setRegDstJump, Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite
    // HALT if opcode is NONE of the 14 allowed, return 1
}

// Sets incoming data to Registers 1 & 2
/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    data1 = Reg[r1];
    data2 = Reg[r2];
}

// Sign-extend 16-bit offset → 32-bit integer.
/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //If offset bit 15 is 1 → fill top bits with 1’s.
}

// Decides what the ALU should do and runs ALU
/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    /*
    1. Choose the correct second operand for the ALU
        If ALUSrc == 0 → use data2
        If ALUSrc == 1 → use extended_value (for addi, lw, sw) or 
        OR funct if 
    2. Determine which ALU operation to perform
        ALUOp (telling you the ALU operation category)
        funct field (only used for R-type instructions)
    3. Call ALU
    4. Return HALT condition
    */
}

// Load from memory or store to memory safely
/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    /*
    If MemRead == 1:
        ensure address is word aligned
        ensure address is legal(within memory bounds 0-65535)
        memdata = Mem[ALUresult >> 2]
    If MemWrite == 1:
        ensure address is aligned
        ensure address is legal (within memory bounds 0–65535)
        Mem[ALUresult >> 2] = data2
    else do nothing
    If illegal memory access → return 1 (HALT)
    else return 0
    */
}

// Write the final result back to a register
/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    /*
    If RegWrite == 1:
        Choose destination:
            if RegDst == 1 → r3
            if RegDst == 0 → r2
        Choose value:
            if MemtoReg == 1 → memdata
            else → ALUresult
        Write to Reg[dest], unless dest = $0. $0 must never change
    */
}


//Compute the next program counter
/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    /*
    Perform:
        PC = PC + 4
        If Branch == 1 AND Zero == 1:
        PC += extended_value << 2
        If Jump == 1:
        PC = (PC & 0xF0000000) | (jsec << 2);
    */
}

