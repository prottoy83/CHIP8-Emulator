#include <cstdio>
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

        //  Graphics Layout
        unsigned char gfx[2048];        // 1bit [Black And White], 64x32 = 2048p;

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
                        case 0x00E0:                // Graphics buffer clear | CLS
                            for(int i =0; i<2048;i++) 
                                gfx[i] = 0;
                            pc+=2;
                            break;

                        case 0x00EE:                // Return from subroutine | RET
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
                
                case 0xA000:                        //ANNN : LD I, addr
                    I = op & 0x0FFF;
                    pc += 2;
                    break;


                default:
                    printf("OP ERROR: 0x%04X at PC=0x%03X", op, pc);
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

};

