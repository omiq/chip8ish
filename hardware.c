// RAM is 4kb, with first 512 bytes reserved
// for the original interpreter 
unsigned char ram[4096];

// There are 16 registers
// V0 - VF
unsigned char registers[16];
// VF register should not be used by 
// programs, it's used for instructions

// Special register  I
// This register is used to store memory addresses, 
// lowest (rightmost) 12 bits are used.
unsigned int I;

// delay and sound registered
unsigned char delay, sound;

// Program Counter
unsigned int PC;

// Stack Pointer
unsigned char SP;

// The stack is an array of 16x16-bit values
unsigned int stack[16];

// Original: 64x32-pixel monochrome display
// Super Chip-48: 128x64
unsigned char display[128][64];

// Chip-8 timers: delay and sound
unsigned int delay, sound;



/*
Chip-8 expects Character 'Sprites' 0-F
8x5 pixels to be stored in 0x000 to 0x1FF
(the interpreter area)
*/

