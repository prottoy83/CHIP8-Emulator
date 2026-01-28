cmake_minimum_required(VERSION 3.20)

project(CHIP8_Emulator)

add_executable(Emu_CHIP8 
    main.cpp
)

target_include_directories(Emu_CHIP8
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_directories(Emu_CHIP8
    PRIVATE
        ${PROJECT_SOURCE_DIR}/lib
)

target_link_libraries(Emu_CHIP8
    PRIVATE
        SDL2
)

if (MINGW)
    target_compile_definitions(Emu_CHIP8 PRIVATE SDL_MAIN_HANDLED)
    target_link_options(Emu_CHIP8 PRIVATE -mconsole)
endif()