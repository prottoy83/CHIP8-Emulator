# CHIP-8 Emulator (C++ / SDL2)

A simple **CHIP-8 emulator** written in modern **C++** using **SDL2** for windowing, input, and audio.

This project focuses on learning **CPU emulation, low-level systems, and virtual machine design**, starting from a clean interpreter core and gradually adding graphics, timers, input, and sound.

Inspired by:  
https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

---

## Features (Work In Progress)

### Implemented
- CHIP-8 memory model (4 KB)
- Registers (V0–VF, I, PC, SP)
- Opcode fetch/decode cycle
- Fontset loading
- SDL2 window + basic event loop
- CMake + Ninja build system

### In Progress
- Full opcode implementation
- Graphics rendering (64×32 framebuffer → scaled texture)
- Keyboard input mapping
- 60 Hz timers
- Sound (buzzer)
- ROM loader

### Planned
- Instruction timing control
- Debug logging
- Step-through debugger mode
- Super CHIP (128×64) support
- Cross-platform builds

---

## Tech Stack

- **Language:** C++17+
- **Window/Input/Audio:** SDL2
- **Build System:** CMake
- **Build Backend:** Ninja

---

## Project Structure

CHIP8-Emulator/
│
├─ src/
│ ├─ main.cpp
│ ├─ chip8.h
│ └─ chip8.cpp
│
├─ CMakeLists.txt
└─ build/


---

## Build Instructions

### Requirements
- CMake ≥ 3.20
- Ninja
- SDL2 development package
- C++ compiler (MSVC / g++ / clang)

---

## Windows (MSVC + Ninja)

### 1. Install SDL2
Download:

SDL2-devel-2.x.x-VC.zip

from: https://libsdl.org

After extracting:
- add `include/` to include path
- add `lib/x64/` to library path
- copy `SDL2.dll` next to your `.exe`

---

### 2. Build

```bash
mkdir build
cd build
cmake -G Ninja ..
ninja
```

```bash
./CHIP8-Emulator.exe
```

---
# Controls

```bash
Keyboard      CHIP-8
1 2 3 4   →    1 2 3 C
Q W E R   →    4 5 6 D
A S D F   →    7 8 9 E
Z X C V   →    A 0 B F
```
# How CHIP-8 Works (Quick Overview)
- 4 KB memory
- Programs start at 0x200
- 16 8-bit registers (V0–VF)
- 16-level stack
- 64×32 monochrome display
- 60 Hz delay & sound timers

```bash
fetch → decode → execute → update timers → render → input
```

# References
- Cowgod’s CHIP-8 Technical Reference
- Multigesture CHIP-8 tutorial
- SDL2 Documentation

# License
- MIT License

# Status

This emulator is under active development.
Core CPU features are being implemented first before graphics and audio.

Contributions, suggestions, and improvements are welcome.