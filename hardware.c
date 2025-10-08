#include <stdbool.h>
#include <stdio.h>

// RAM is 4kb, with first 512 bytes reserved
// for the original interpreter 
unsigned char ram[4096];

// There are 16 registers
// V0 - VF
unsigned char V[16];
// VF register should not be used by 
// programs, it's used for instructions

// Special register  I
// This register is used to store memory addresses, 
// lowest (rightmost) 12 bits are used.
unsigned int I;

// delay and sound registers
unsigned char delay, sound;

// Program Counter
unsigned int PC;

// The stack is an array of 16x16-bit values
unsigned char SP; // Stack Pointer
unsigned int stack[16];

// Original: 64x32-pixel monochrome display
// Super Chip-48: 128x64
unsigned char screen[128][64];
bool redraw = 0; // Should update the screen

// Chip-8 timers: delay and sound
unsigned int Tdelay, Tsound;



/*
Chip-8 expects Character 'Sprites' 0-F
8x5 pixels to be stored in 0x000 to 0x1FF
(the interpreter area)
*/
void create_sprites(void) {

unsigned char P = 0;

// "0"
    ram[P] = 0b11110000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11110000;

// "1"
    ram[++P] = 0b00100000;
    ram[++P] = 0b01100000;
    ram[++P] = 0b00100000;
    ram[++P] = 0b00100000;
    ram[++P] = 0b01110000;

// "2"
    ram[++P] = 0b11110000;
    ram[++P] = 0b00010000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b11110000;

// "3"
    ram[++P] = 0b11110000;
    ram[++P] = 0b00010000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b00010000;
    ram[++P] = 0b11110000;

// "4"
    ram[++P] = 0b10010000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b00010000;
    ram[++P] = 0b00010000;

// "5"
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b00010000;
    ram[++P] = 0b11110000;

// "6"
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11110000;

// "7"	
    ram[++P] = 0b11110000;
    ram[++P] = 0b00010000;
    ram[++P] = 0b00100000;
    ram[++P] = 0b01000000;
    ram[++P] = 0b01000000;

// "8"	
    ram[++P] = 0b11110000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11110000;

// "9"	
    ram[++P] = 0b11110000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b00010000;
    ram[++P] = 0b11110000;

// "A"	
    ram[++P] = 0b11110000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b10010000;

// "B"	
    ram[++P] = 0b11100000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11100000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11100000;

// "C"	
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b11110000;

// "D"	
    ram[++P] = 0b11100000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b10010000;
    ram[++P] = 0b11100000;

// "E"	
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b11110000;

// "F"	
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b11110000;
    ram[++P] = 0b10000000;
    ram[++P] = 0b10000000;

}

/*

Fetch-Decode-Execute

*/

// The 'decode' part
void parser(int instruction) {

    //   The full instruction is supplied as 16 bits
    //   We need to split the opcode and data
    unsigned int opcode;
    unsigned int data;
    opcode = instruction & 0xF000;
    data = instruction & 0x0FFF;

    /*
        Standard Chip-8 Instructions
            00E0 - CLS
            00EE - RET
            0nnn - SYS addr
            1nnn - JP addr
            2nnn - CALL addr
            3xkk - SE Vx, byte
            4xkk - SNE Vx, byte
            5xy0 - SE Vx, Vy
            6xkk - LD Vx, byte
            7xkk - ADD Vx, byte
            8xy0 - LD Vx, Vy
            8xy1 - OR Vx, Vy
            8xy2 - AND Vx, Vy
            8xy3 - XOR Vx, Vy
            8xy4 - ADD Vx, Vy
            8xy5 - SUB Vx, Vy
            8xy6 - SHR Vx {, Vy}
            8xy7 - SUBN Vx, Vy
            8xyE - SHL Vx {, Vy}
            9xy0 - SNE Vx, Vy
            Annn - LD I, addr
            Bnnn - JP V0, addr
            Cxkk - RND Vx, byte
            Dxyn - DRW Vx, Vy, nibble
            Ex9E - SKP Vx
            ExA1 - SKNP Vx
            Fx07 - LD Vx, DT
            Fx0A - LD Vx, K
            Fx15 - LD DT, Vx
            Fx18 - LD ST, Vx
            Fx1E - ADD I, Vx
            Fx29 - LD F, Vx
            Fx33 - LD B, Vx
            Fx55 - LD [I], Vx
            Fx65 - LD Vx, [I]
       Super Chip-48 Instructions
            00Cn - SCD nibble
            00FB - SCR
            00FC - SCL
            00FD - EXIT
            00FE - LOW
            00FF - HIGH
            Dxy0 - DRW Vx, Vy, 0
            Fx30 - LD HF, Vx
            Fx75 - LD R, Vx
            Fx85 - LD Vx, R
    */

    // Decides what to do with the next instruction
    switch (opcode) {

    // ..............................................

    // Only two opcodes affect the screen
    // and need to set the draw flag

    // Clear the screen    
    case 0x00E0: 
        break;
    
    // 0xDXYN Draw a sprite
    case 0xD000:
        break;
    
    // ..............................................
    
    // Return from subroutine    
    case 0x00EE: 
        break;
    
    // Set I to supplied address
    case 0xA000:
        I = data;
        break;
        
    default:
        printf ("opcode: %X\n", opcode);
    }

    // Debug
    printf ("Instruction: %X %X\n", opcode, data);


    // This won't always be true
    // but for now we can just inc PC
    PC += 2;

}

void update_screen() {
    
    unsigned char x,y=0;
    // Here we will update the screen
    // for now use characters
    for(y=0; y<64; y++)
    {
        for(x=0; x<64; x++)
        {
            putchar(screen[x][y]);
        }
    }


}

void main(void) {

    // Temp variables
    unsigned char current_byte = 0;
    unsigned char next_byte = 0;
    unsigned int combined_instruction = 0;

    // the machine is powered up
    bool powered_on = true;

    // Set up the default character sprites
    create_sprites();
    
    // Program counter reset to zero
    PC=0;

    parser(0b1010001011110000);
    powered_on = false;

    // While the machine is powered up
    while(powered_on) {

        // One CPU cycle:
        // . Fetch the first byte of the opcode
        current_byte = ram[PC];
        //   one opcode is 2 bytes so we also need the next byte
        next_byte = ram[PC+1];

        // Now we need to combine the two bytes into a 16-bit big endian
        combined_instruction = (current_byte << 8 | next_byte);

        // . Decode using parser
        parser(combined_instruction);

        // . Execute instruction
        // . Update timers


        // If the draw flag is set, update the screen
        if(redraw) update_screen();
 
        // Check keys


    }

    return;
}