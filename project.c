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

// Takes PC to check word-alignment, memory range verification, 
// where word is loaded properly, halt if illegal
/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    //Checks word Alignment (factor of 4)
    if (PC & 0x3) {
        return 1;
    }

    //Checks if PC is within 64KB range
    if (PC > 0xFFFF){
        return 1;
    }
    //Fetches after checking
    //Mem[PC >> 2] is derefrencing to Mem
    *instruction = Mem[PC >> 2];

    return 0;
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
    *op = instruction >> 26;
    *r1 = (instruction >> 21) & 0x1F;
    *r2 = (instruction >> 16) & 0x1F;
    *r3 = (instruction >> 11) & 0x1F;
    *funct = instruction & 0x3F;   // 0x3F = 6 bits of 1’s
    *offset = instruction & 0xFFFF;   // 16 bits
    *jsec = instruction & 0x3FFFFFF;
}


//Sets control signals
/* instruction decode */
/* 15 Points */
    int instruction_decode(unsigned op, struct_controls *controls)
{
    /*
    Use the instruction list in PDF Appendix A.
    Read op and setRegDstJump, Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite
    HALT if opcode is NONE of the 14 allowed, return 1
    */

    // Default all control signals to 0
    controls->RegDst   = 0;
    controls->Jump     = 0;
    controls->Branch   = 0;
    controls->MemRead  = 0;
    controls->MemtoReg = 0;
    controls->ALUOp    = 0;
    controls->MemWrite = 0;
    controls->ALUSrc   = 0;
    controls->RegWrite = 0;

    switch (op) {
    case 0x00: // R-type
        controls->RegDst = 1;
        controls->RegWrite = 1;
        controls->ALUSrc = 0;
        controls->ALUOp = 0x7; // R-type
        break;

    case 0x23: // lw
        controls->RegDst = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 0;
        break;

    case 0x2B: // sw
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 0;
        //Don't care cases, not needed but included
        controls->RegDst   = 2; // don't care
        controls->MemtoReg = 2; // don't care
        break;

    case 0x04: // beq
        controls->Branch = 1;
        controls->ALUSrc = 0;
        controls->ALUOp = 1;
        //Don't care cases, not needed but included
        controls->RegDst   = 2; // don't care
        controls->MemtoReg = 2; // don't care
        break;

    case 0x02: // j
        controls->Jump = 1;
        //Don't care cases, not needed but included
        controls->RegDst   = 2; // don't care
        controls->MemtoReg = 2; // don't care
        controls->ALUSrc   = 2; // don't care
        controls->ALUOp    = 0; // don't care
        break;

    case 0x08: // addi
        controls->RegDst = 0;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 0;
        break;
    case  0x0F: // lui
        controls->RegDst   = 0;   // destination is rt
        controls->RegWrite = 1;   // write to register
        controls->ALUSrc   = 1;   // use immediate
        controls->MemRead  = 0;
        controls->MemWrite = 0;
        controls->MemtoReg = 0;   // write ALU result
        controls->Branch   = 0;
        controls->Jump     = 0;
        controls->ALUOp    = 6;   // 110: shift left extended_value by 16
        break;

    default:
        return 1; // illegal instruction → halt
    }

    return 0;
}

// Sets incoming data to Registers 1 & 2
/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    //Pointer since we're trying to write to that location
    // Read value in register r1 into data1
    *data1 = Reg[r1];
    // Read value in register r2 into data2
    *data2 = Reg[r2];
}

// Sign-extend 16-bit offset → 32-bit integer.
/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //If offset bit 15 is 1 → fill top bits with 1’s.
        // Check if the sign bit (bit 15) is 1
    //Checks offset against mask 15th bit 0x8000
    if (offset & 0x8000) {
        // Negative number → extend with 1's
        //OR bc 0 OR X = X also 1 OR X = 1
        *extended_value = offset | 0xFFFF0000;
    }
    else {
        // Positive number → extend with 0's
        //AND bc 0 AND X = 0 also 1 AND X = X
        *extended_value = offset & 0xFFFF;
    }
}

// Decides what the ALU should do and runs ALU
/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
    unsigned operand2;
    unsigned ALUControl;

    /* Select second ALU operand based on ALUSrc */
    if (ALUSrc == 1) {
        operand2 = extended_value;   // immediate path
    } else {
        operand2 = data2;         // register path
    }

    /* Decide ALUControl based on ALUOp (and funct for R-type) */
    switch (ALUOp) {
        case 0:  // ADD (addi, lw, sw)
            ALUControl = 0;
            break;

        case 1:  // SUB (beq)
            ALUControl = 1;
            break;

        case 2:  // SLT immediate
            ALUControl = 2;
            break;

        case 3:  // SLTU immediate
            ALUControl = 3;
            break;

        case 4:  // AND immediate
            ALUControl = 4;
            break;

        case 5:  // OR immediate
            ALUControl = 5;
            break;

        case 6:  // LUI: shift left 16
            ALUControl = 6;
            break;

        case 7:  // R-type: use funct field
            switch (funct) {
                case 32: // 0x20: add
                    ALUControl = 0;
                    break;
                case 34: // 0x22: sub
                    ALUControl = 1;
                    break;
                case 36: // 0x24: and
                    ALUControl = 4;
                    break;
                case 37: // 0x25: or
                    ALUControl = 5;
                    break;
                case 42: // 0x2A: slt
                    ALUControl = 2;
                    break;
                case 43: // 0x2B: sltu
                    ALUControl = 3;
                    break;
                default:
                    return 1;  // illegal R-type funct
            }
            break;

        default:
            return 1;  // illegal ALUOp
    }

    /* Actually perform the ALU operation */
    ALU(data1, operand2, ALUControl, ALUresult, Zero);

    return 0;  // no halt
}


// Load from memory or store to memory safely
/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if (MemRead == 0 && MemWrite == 0) {
        // No memory operation, just return success
        return 0;
    }
    // Check if address is within valid range
    if (ALUresult > 0xFFFF) {
        return 1; // Halt - address out of range
    }
    
    // Check word alignment
    if (ALUresult % 4 != 0) {
        return 1; // Halt - not word aligned
    }
    
    // Convert byte address to word index
    unsigned word_index = ALUresult >> 2;
    
    if (MemRead == 1) {
        // Load word from memory
        *memdata = Mem[word_index];
    }
    
    if (MemWrite == 1) {
        // Store word to memory
        Mem[word_index] = data2;
    }
    
    return 0; // Success
}

// Write the final result back to a register
/* Write Register */
/* 10 Points */

void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
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

    if (RegWrite == 1) {

        unsigned dest;
        unsigned val;

        //Choose destination

        if (RegDst == 1) {
            dest = r3;
        }
        else if (RegDst == 0)
            dest = r2;

        //$0 does not change
        if (dest == 0) {
            return;
        }

        //Choose value
        if (MemtoReg == 1) {
            val = memdata;
        }
        else
            val = ALUresult;

        //Write to Reg[dest]
        Reg[dest] = val;
    } 
}


//Compute the next program counter
/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	// Every cycle, PC increments by 4 
	*PC = *PC + 4;
	
	// Branch beq 
	if (Branch == 1 && Zero == 1)
	{
		// offset is word-based so we shift left 2 bits
		*PC = *PC + (extended_value << 2);
	}	
    // Jump
	if (Jump == 1)
	{
		// Replace low 28 bits with jsec << 2, keep upper 4 bits
		*PC = (*PC & 0xF0000000) | (jsec << 2);
	}
}
