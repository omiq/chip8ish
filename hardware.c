#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
unsigned char screen_width = 64;
unsigned char screen_height = 32;
bool screen[(64*32)];
bool redraw = false; // Should update the screen

// Chip-8 timers: delay and sound
unsigned int Tdelay, Tsound;

void clear_regs() {
    
    unsigned char i=0;
    // Here we will update the screen
    // for now use characters
    for(i=0; i<sizeof(V); i++)
    {
        V[i]=0;
    };
}

void clear_ram() {
    
    unsigned int i=0;
    // Here we will update the screen
    // for now use characters
    for(i=0; i<sizeof(ram); i++)
    {
        ram[i]=0;
    };
}

void clear_stack() {
    
    unsigned int i=0;
    // Here we will update the screen
    // for now use characters
    for(i=0; i<sizeof(stack); i++)
    {
        stack[i]=0;
    };
}

void clear_screen() {
    
    unsigned int i=0;
    // Here we will update the screen
    // for now use characters
    for(i=0; i<sizeof(screen); i++)
    {
        screen[i]=0;
    };

    redraw = false; 
}

/* =======[keys]==========*/
/* chip8 uses a calculator-like hex numeric keypad like the kim1 */
unsigned char keys[16];	

void keyboardDown(unsigned char key, int x, int y)
{
	if(key == 27)    // escape
		exit(0);

	if(key == '1')		keys[0x1] = 1;
	else if(key == '2')	keys[0x2] = 1;
	else if(key == '3')	keys[0x3] = 1;
	else if(key == '4')	keys[0xC] = 1;

	else if(key == 'q')	keys[0x4] = 1;
	else if(key == 'w')	keys[0x5] = 1;
	else if(key == 'e')	keys[0x6] = 1;
	else if(key == 'r')	keys[0xD] = 1;

	else if(key == 'a')	keys[0x7] = 1;
	else if(key == 's')	keys[0x8] = 1;
	else if(key == 'd')	keys[0x9] = 1;
	else if(key == 'f')	keys[0xE] = 1;

	else if(key == 'z')	keys[0xA] = 1;
	else if(key == 'x')	keys[0x0] = 1;
	else if(key == 'c')	keys[0xB] = 1;
	else if(key == 'v')	keys[0xF] = 1;

	
}

void keyboardUp(unsigned char key, int x, int y)
{
	if(key == '1')		keys[0x1] = 0;
	else if(key == '2')	keys[0x2] = 0;
	else if(key == '3')	keys[0x3] = 0;
	else if(key == '4')	keys[0xC] = 0;

	else if(key == 'q')	keys[0x4] = 0;
	else if(key == 'w')	keys[0x5] = 0;
	else if(key == 'e')	keys[0x6] = 0;
	else if(key == 'r')	keys[0xD] = 0;

	else if(key == 'a')	keys[0x7] = 0;
	else if(key == 's')	keys[0x8] = 0;
	else if(key == 'd')	keys[0x9] = 0;
	else if(key == 'f')	keys[0xE] = 0;

	else if(key == 'z')	keys[0xA] = 0;
	else if(key == 'x')	keys[0x0] = 0;
	else if(key == 'c')	keys[0xB] = 0;
	else if(key == 'v')	keys[0xF] = 0;
}



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


// The 'decode' and 'excute' parts
void interpreter(int instruction) {



    /*
        Standard Chip-8 Instructions
						
        00E0 - CLS		        8xy0 - LD Vx, Vy		Fx07 - LD Vx, DT		## Super Chip-48 Instructions ##
        00EE - RET		        8xy1 - OR Vx, Vy		Fx0A - LD Vx, K		    00Cn - SCD nibble
        0nnn - SYS addr		    8xy2 - AND Vx, Vy		Fx15 - LD DT, Vx		00FB - SCR
                                8xy3 - XOR Vx, Vy		Fx18 - LD ST, Vx		00FC - SCL
        1nnn - JP addr		    8xy4 - ADD Vx, Vy		Fx1E - ADD I, Vx		00FD - EXIT
                                8xy5 - SUB Vx, Vy		Fx29 - LD F, Vx		    00FE - LOW
        2nnn - CALL addr		8xy6 - SHR Vx {, Vy}	Fx33 - LD B, Vx		    00FF - HIGH
                                8xy7 - SUBN Vx, Vy		Fx55 - LD [I], Vx		Dxy0 - DRW Vx, Vy, 0
        3xkk - SE Vx, byte		8xyE - SHL Vx {, Vy}	Fx65 - LD Vx, [I]		Fx30 - LD HF, Vx
                                                        Fx75 - LD R, Vx
        4xkk - SNE Vx, byte		9xy0 - SNE Vx, Vy		Fx85 - LD Vx, R
        5xy0 - SE Vx, Vy		Annn - LD I, addr				
        6xkk - LD Vx, byte						
        7xkk - ADD Vx, byte		Bnnn - JP V0, addr				
						
		Cxkk - RND Vx, byte				
		Dxyn - DRW Vx, Vy, nibble				
		Ex9E - SKP Vx				
		ExA1 - SKNP Vx				
    */

    //   The full instruction is supplied as 16 bits
    //   We need to split the opcode and data
    unsigned int opcode;
    unsigned int data;
    unsigned char rbits;
    unsigned char reg, reg2;
    unsigned char x,y,height,pixel,row,col;
    opcode = instruction & 0xF000;
    rbits = opcode & 0xF000;
    reg = (opcode & 0x0F00) >> 8; 
    reg2 = (opcode & 0x00F0) >> 4;
    data = opcode & 0x0FFF;

    // For debugging we need to know if we caught the opcode
    bool handled = true;

    // NB: Only two opcodes affect the screen
    //     and need to set the draw flag

    // Decides what to do with the next instruction
    switch (opcode) {

    case 0x0000:
    
        switch(rbits) {
        // NoP

        // 0x00E0
        // Clear the screen    
        case 0x0000: 
            clear_screen();
            break;

        // 0x00EE
        // Return from subroutine    
        case 0x000E: 
            break;
        }



    // Jump    
    case 0x1000: 
        // DON'T add to stack just jump
        PC = data;
        printf("JP: %X\n", PC);
        break;

    // JSR
    case 0x2000:
        stack[SP] = PC;
        SP++;
        PC = data;
        printf("JSR: %X\n", PC);
        break;

    // Set register VX
    case 0x6000:
        V[reg]=data;
        printf("LD V%X, %X\n", reg, data);
        break;
    
    // Add to register VX
    case 0x7000:
        V[reg] += data;
        printf("ADD V%X, %X\n", reg, data);
        break;

    
    // Set I to supplied address
    case 0xA000:
        I = data;
        printf("LD I, %X\n", data);
        break;



    
    // Draw a sprite
    case 0xD000:

        x = V[reg];
        y = V[reg2];
        height = opcode & 0x000F;
        pixel = 0;
        row = 0;
        col = 0;

        // While we have rows to draw
        while(row < height) {
            // Get the pixel data for the current row
            pixel = ram[I + row];

            // While we have columns to draw
            // Each row is 8 pixels wide
            col = 0;
            while(col < 8) {
                // If the pixel is set, XOR the screen
                if(pixel & (0x80 >> col)) {
                    // If the pixel is already set, set the carry flag
                    if(screen[(x + col + ((y + row) * 64))] == 1) V[0xF] = 1;
                    // XOR the pixel (causes flickering)                                   
                    screen[((y + row) * screen_width) + (x + col)] ^= 1;
                }
                col++;
            }
            row++;
        }

        redraw = true; 
        break;
    
    // ..............................................
    




    default:
        // Debug
        printf ("PC:%X Unhandled Instruction: Op:%X Reg:%X Rbits:%X Data:%X\n", PC, opcode, reg, rbits, data);
        handled=false;

    }

    if(handled) printf ("PC:%X Success! Instruction: Op:%X Reg:%X Rbits:%X Data:%X\n", PC, opcode, reg, rbits, data);

    // This won't always be true
    // but for now we can just inc PC
    PC += 2;

}


void update_screen() {
    
    unsigned char x=0,y,i=0;


    //printf("\33[2J\n");
    // Here we will update the screen
    // for now use characters
    y = 0;
    
    for(y = 0; y < screen_height; y++)
    {
        printf("\n%d ", y);
        for(x = 0; x < screen_width; x++)
        {
            if(screen[(y * screen_width) + x]) printf("1"); else printf("0");
        }
        

    };

    redraw = false; 

    printf("\n");
}


void printbin(unsigned char value) {
    for(int i = 7; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
    printf("\n");
}

void reset(void) {

    printf("Starting ...\n");

    // Initialise variables
    clear_ram(); // 4kb to 0
    clear_regs(); // 16 bytes to 0
    clear_stack(); // clear the stack
    clear_screen(); // wipe the screen memory



    I = 0;
    SP = 0;
    Tdelay = Tsound = delay = sound = 0;

    // Set up the default character sprites
    create_sprites();

    // CHIP8 looks for programs at 0x200
    PC = 0x200;

    // Start the screen
    update_screen();
}

// One CPU cycle:
void single_step(void) {

    // Temp variables
    unsigned char current_byte = 0;
    unsigned char next_byte = 0;
    unsigned int combined_instruction = 0;

        //   one opcode is 2 bytes so we also need the next byte
        current_byte = ram[PC];
        next_byte = ram[PC+1];

        // Now we need to combine the two bytes into a 16-bit big endian
        combined_instruction = (current_byte << 8 | next_byte);

        // 2. Decode using parser
        // 3. Execute instruction
        // . Update timers
        interpreter(combined_instruction);
}

bool load(const char* filename)
{

    unsigned int i;

	// Open file
	FILE * pFile = fopen(filename, "rb");
	if (pFile == NULL)
	{
		printf("Error loading %s \n", filename);
		return false;
	} else {
        // Attempt to load a file into chip8 ram
        printf("Load file %s\n", filename);
    	
    }


	// Check file size
	fseek(pFile , 0 , SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("File Size is: %d\n", (int)lSize);
	
	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL) 
	{
		printf("Error allocating memory!\n"); 
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread (buffer, 1, lSize, pFile);
	if (result != lSize) 
	{
		printf("Error reading file!\n"); 
		return false;
	}

	// Copy buffer to Chip8 memory
	if((4096-512) > lSize)
	{
		for(i = 0; i < lSize; ++i)
        {
            // programs go into 0x200
			ram[0x200+i] = buffer[i];
            printbin(buffer[i]);
        }
	}
	else
    {
		printf("Uh-oh! Out of memory loading program %ld!\n",lSize); 
        exit(1);
    }    
    // Clean up
	fclose(pFile);
	free(buffer);

    // It worked!
	return true;
}

int main(int argc, char *argv[]) {

    unsigned char c=0;

    // Single or Automatic operation
    bool auto_mode = true;

    // the machine is powered up
    bool powered_on = true;

    // Program counter reset to zero
    PC=0;


    // Help and argument parsing
    if(argc>1) {
        // printf("\n\nCHIP8ish called with %d arguments:\n", argc);
        // for (int i = 0; i < argc; i++) {
        //     printf("%s\n", argv[i]);
        // }
        if(argc == 3) auto_mode = false;
    }
    else
    {
        printf("\n\nCHIP8ish\n");
        printf("Chris Garrett RetroGameCoders.com\n");
        printf("2025\n\n");
        printf("USAGE: chip8ish [filename]\n\n");
        exit(0);

    }
    
    // Call reset to init the registers etc
    reset();

    // Load the requested program into RAM
    bool success = load(argv[1]);

    // While the machine is powered up  
    while(powered_on && PC < sizeof(ram) && auto_mode == 1) {

        single_step();

        // If the draw flag is set, update the screen
        if(redraw) update_screen();
 
        // Check keys

    }

    if(!auto_mode) {
        while(c!=27) {
            printf("\n\rSINGLE STEP TEST\n\r");
            // ram[PC]=0xA2;
            // ram[PC+1]=0xF0;
            single_step();
            if(redraw) update_screen();
            c=getchar();
        }
    }

    printf("\n\n");
    return 0;
}