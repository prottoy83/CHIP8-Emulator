#include <cstdio>
#include <cstdlib>
#include <ctime>

class Chip8{
    private:
        //  CPU Specification
        unsigned short opcode;          //operation code
        unsigned char memory[4096];     //4KB memory 
        unsigned char Reg[16];          //15 registers [V00-V15]; Additional Carry bit
        unsigned short I;               //Index register
        unsigned short pc;              //Program Counter
        unsigned short stack[16];       //Chip Stack

        /*  MEMORY LAYOUT
            0x000 - 0x1FF => CHIP 8 Interpreter
            0x050 - 0x0A0 => 4x5 Pixel font set
            0x200 - 0xFFF => Program 
        */

        //  Graphics
        unsigned char gfx[2048];        // 1bit [Black And White], 64x32 = 2048p;
        bool drawFlag = false;          // Sets the state to draw in screen
        

        //  Timers
        unsigned char delay_timer;      //Event timer for games
        unsigned char sound_timer;      //Timer for sound effects

        //  Control keys
        unsigned char key[16];          // HEX Based Keypad (0x0-0xF)

        // Stack Pointer
        unsigned char sp;

        //CHIP8 FONT SET
        unsigned char chip8_fontset[80] =
        { 
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        // Extra variable to control data;
            unsigned int soundPlay = 00;

        void decode(unsigned op){

            // Operations
            unsigned short A = op & 0xF000;             // Instruction op
            unsigned short B = (op & 0x0F00) >> 8;      // x register
            unsigned short C = (op & 0x00F0) >> 4;      // y register
            unsigned short D = op & 0x000F;             // nibble
            unsigned short kk = op & 0x00FF;            //byte
            unsigned short nnn = op & 0x0FFF;           // address

            switch(op & 0xF000)
            {
                case 0x0000:
                    switch(op & 0x00FF){
                        case 0xE0:                // Graphics buffer clear | CLS
                            for(int i =0; i<2048;i++) 
                                gfx[i] = 0;
                            pc+=2;
                            break;

                        case 0xEE:                // Return from subroutine | RET
                            sp--;               
                            pc = stack[sp];
                            stack[sp]=0;
                            break;
                        
                        default:                    // Default program counter addition
                            pc+=2;
                            break;
                    }
                    break;
                
                case 0x1000:                        // Set program counter to 0x1(Address) | JP addr
                    pc = nnn;
                    break;
                
                case 0x2000:                        // Increment stack adder | CALL addr
                    stack[sp] = pc + 2;
                    sp++;
                    pc=nnn;
                    break;
                
                case 0x3000:                        //Compares Vx to kk; on equal skips next instruction | SE Vx, byte
                    pc += (Reg[B] == kk) ? 4:2;
                    break;
                
                case 0x4000:                        //Compares Vx to kk; on not equal skips next instruction | SE Vx, byte
                    pc += (Reg[B] != kk) ? 4:2;
                    break;
                
                case 0x5000:                        //Compares Vx to Vy; on equal skips next instruction | SE Vx, Vy
                    pc += (Reg[B] == Reg[C]) ? 4:2;
                    break;
                
                case 0x6000:                        // Set Vx = kk | SE Vx, Vy
                    Reg[B] = kk;
                    pc += 2;
                    break;

                case 0x7000:                        // Add kk to Vx | SE Vx, byte
                    Reg[B] = Reg[B]+kk;
                    pc += 2;
                    break;

                case 0x8000:
                    switch(D){
                        case 0x0:                   // Set Vx = Vy | LD Vx, Vy
                            Reg[B] = Reg[C];
                            break;
                        case 0x1:                   // Performs OR between reg X, Y | OR Vx, Vy
                            Reg[B] = Reg[B] | Reg[C];
                            break;
                        case 0x2:                   // Performs AND between reg X, Y | AND Vx, Vy
                            Reg[B] = Reg[B] & Reg[C];
                            break;
                        case 0x3:                   // Performs XOR between reg X, Y | XOR Vx, Vy
                            Reg[B] = Reg[B] ^ Reg[C];
                            break;
                        case 0x4:{                  // Performs Addition reg X, Y | ADD Vx, Vy
                            unsigned short sum = Reg[B] + Reg[C];
                            Reg[0xF] = (sum>255);   // carry bit
                            Reg[B] = sum & 0xFF;    // 8 bits
                            break;
                        }
                        case 0x5:                   // Performs Subtraction reg X, Y | SUB Vx, Vy
                            Reg[0xF] = (Reg[B] > Reg[C]); // Do not borrow 
                            Reg[B] = Reg[B] - Reg[C];
                            break;
                        case 0x6:                   // Set Vx = Vx SHR 1 | SHR Vx {, Vy}
                            Reg[0xF] = Reg[B] & 0x1;
                            Reg[B] >>= 1;
                            break;
                        case 0x7:                   // Set Vx = Vy - Vx, set VF = NOT borrow. | SUBN Vx, Vy
                            Reg[0xF] = (Reg[C] > Reg[B]) ? 1:0;
                            Reg[B] = Reg[C] - Reg[B];
                            break;
                        case 0xE:                   // Set Vx = Vy - Vx, set VF = NOT borrow. | SUBN Vx, Vy
                            Reg[0xF] = (Reg[B] & 0x80) >> 7; // MSB before shift
                            Reg[B] <<= 1;
                            break;
                    }
                    pc += 2;
                    break;
                
                case 0x9000:                        // Skip next instruction if Vx != Vy | SNE Vx, Vy
                    if(D == 0 && Reg[B] != Reg[C]){
                        pc+=4;
                    }
                    else{
                        pc+=2;
                    }
                    break;
                
                case 0xA000:                        //ANNN : LD I, addr
                    I = op & 0x0FFF;
                    pc += 2;
                    break;
                
                case 0xB000:                        // Jump to location nnn + V0.
                    pc = nnn+Reg[0];
                    break;

                case 0xC000: {                      // Set Vx = random byte AND kk.
                    unsigned char random_byte = rand() & 0xFF;  // generates a random number from 0 to 255
                    Reg[B] = random_byte & kk;
                    pc+=2;
                    break;
                }
                
                case 0xD000:{
                    /*
                    The interpreter reads n bytes from memory, starting at the address stored in I.
                    These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
                    Sprites are XORed onto the existing screen. If this causes any pixels to be 
                    erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so 
                    part of it is outside the coordinates of the display, it wraps around to the 
                    opposite side of the screen.
                    */

                    drawFlag = true;

                    unsigned char x = Reg[B] % 64;              // X axis resets after 64 pixels
                    unsigned char y = Reg[C] % 32;              // Y axis resets after 32 pixels
                    unsigned char height = D;

                    Reg[0xF] = 0;                               // Clears VF 

                    for(int yl = 0 ; yl < height; yl++){        // Draws y line till the sprite reaches height
                        
                        unsigned char spriteBytes = memory[I + yl];  // Reads sprite from memory
                        
                        for( int xl = 0; xl < 8; xl++){
                                if((spriteBytes & (0x80 >> xl)) != 0){  // Reads bits left to right
                                    int xPos = (x+xl) % 64;     // Sets X Axis 
                                    int yPos = (y+yl) % 32;     // Sets Y Axis 
                                    int index = yPos * 64 + xPos;

                                    if(gfx[index] == 1){        // Collision Detection
                                        Reg[0xF] = 1;           // Carry graphics
                                    }

                                    gfx[index] ^=1;             // Toggle Pixels
                                }
                        }
                    }

                    pc+= 2;
                    break;
                }

                default:
                    printf("OP ERROR: 0x%04X at PC=0x%03X\n", op, pc);
                    pc+=2;
                    break;
            }
        }

    public:
        /// Create instance
        Chip8(){
            setvbuf(stdout, nullptr, _IONBF, 0);
            puts("Initialized Chip 8");
        }

        /// Initialize Emulator
        void init(){
            
            // Generate random seed
            srand(time(nullptr));

            // Clear Registers
            for (int i = 0; i < 4096; i++) memory[i] = 0;
            for (int i = 0; i < 16; i++) Reg[i] = key[i] = 0;
            for (int i = 0; i < 2048; i++) gfx[i] = 0;
            for (int i = 0; i < 16; i++) stack[i] = 0;

            pc = 0x200;                                 // Reset Program Counter
            opcode = 0;                                 // Reset current op code
            I = 0;                                      // Reset Current Index Pointer
            sp = 0;                                     // Reset Current Stack Pointer

            delay_timer = 0;                            // Reset delay timer
            sound_timer = 0;                            // Reset sound timer

            for (int i=0; i < 80; i++){                 // Fontset size (5 * 16) = 80 bits
                memory[80 + i]  = chip8_fontset[i];     //Loads Font into the memory after initial 80 bytes
            }
        }

        /// CPU cycle
        void nextCycle(){
            opcode = memory[pc] << 8 | memory[pc+1];    // Get bits from program counter's memory, shifts 8 bit, merge with next bits
            decode(opcode);
            
            if(delay_timer > 0 ){
                --delay_timer;
            }
            if(sound_timer > 0){
                soundPlay = 01;
                printf("BEEP\n");
                --sound_timer;
            }
        }

        /// Load the program
        void loadProgram(unsigned char* buf, int size){
            for(int i = 0; i< size; i++){
                memory[512+i] = buf[i];         //Load the rom's data into memory after inital 512bits
            }
        }

        /// @brief  load the rom
        /// @param filename 
        /// @return 
        bool loadROM(const char* filename) {
            FILE* f = fopen(filename, "rb");
            if (!f) return false;

            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            rewind(f);

            if (size <= 0 || size > (4096 - 0x200)) {
                fclose(f);
                return false;
            }

            size_t read = fread(&memory[0x200], 1, (size_t)size, f);
            fclose(f);

            return read == (size_t)size;
        }

        const unsigned char* getGfx() const { return gfx; }         // Get Graphics
        bool shouldDraw() const { return drawFlag; }                // Get Draw Flag
        void clearDrawFlag() { drawFlag = false; }                  // Set Draw Flag to false

        void setKey(unsigned idx, unsigned char pressed) {          // Key Press (CHATGPT FOR NOW, WILL REPLACE LATER)
            if (idx < 16) key[idx] = pressed;
        }
};

