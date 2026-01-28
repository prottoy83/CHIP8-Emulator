#include <cstdio>
class Chip8{
    private:
        //  CPU Specification
        unsigned short opcode;          //operation code
        unsigned char memory[4096];     //4KB memory 
        unsigned char Reg[16];          //15 registers [V00-V15]; Additional Carry bit
        unsigned short I;               //Index register
        unsigned short pc;              //Program Counter

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

        void decode(unsigned oc){
            switch(oc & 0xF000)
            {
                case 0xA000:
                    I = oc & 0x0FFF;
                    pc += 2;
                    break;


                default:
                    setvbuf(stdout, nullptr, _IONBF, 0);
                    puts("Opcode error");
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
            pc = 0x200;     // Reset Program Counter
            opcode = 0;     // Reset current op code
            I = 0;          // Reset Current Index Pointer
            sp = 0;         // Reset Current Stack Pointer


            for (int i=0; i < 80; i++){             // Fontset size (5 * 16) = 80 bits
                memory[i]  = chip8_fontset[i];      //Loads Font into the memory
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
                --sound_timer;
            }
        }

};

