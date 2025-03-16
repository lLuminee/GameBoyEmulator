#pragma once
#include <iostream>
#include <stdint.h>
#include <vector>
#include <SDL.h>
#include <atomic>

class Cpu
{
private:

    void GetRomName();
    void Get_licensee_code();

public:

    std::vector<std::tuple<uint16_t, uint8_t, std::string, uint16_t>> opcodeHistory; // Historique des opcodes
    std::vector<uint16_t> stack;

    uint8_t state = 1;
    uint16_t LastCalls;
    bool Step = false;
    bool PasCall = false;
    bool ShowVram = false;
    bool IsReset = false;
    std::atomic<bool> isStep = true;   // Contrôle du step pour le CPU
    const char * RomPath;

    SDL_Window* Main_Windo;
    SDL_Window* VRAM_Windo;
    SDL_Renderer* FrameBuffer;
    SDL_Renderer* VRAM_Renderer;


    int pc = 0x0100;
    int sp = 0x0000;
    int last_pc = 0x0100;
    uint8_t id = 0x00;
    uint8_t NextId = 0x00;
    std::string OpcodeName;
    uint32_t opcode;
    uint16_t NextOpcode;
    int OpCycle;

    uint16_t AF = 0x0000; // Accumulator
    uint16_t BC = 0x0000; // B HI C lowssssssssss
    uint16_t DE = 0x0000; // D HI E low
    uint16_t HL = 0x0000; // H HI L low

    uint8_t z = 0x00; // zero flag
    uint8_t n = 0x00; // subtract flag
    uint8_t h = 0x00; // half carry flag
    uint8_t c = 0x00; // carry flag

    uint8_t SCY;
    uint8_t SCX;
    uint8_t LY;
    uint8_t LYC;
    uint8_t WY;
    uint8_t WX;
    bool IME = 0;
    bool IsHalt = 0;
    struct IE_str // Interrupt enable
    {
        bool VBlank;
        bool LCD;
        bool Timer;
        bool Serial;
        bool Joypad;
    };
    struct IF_str // Interrupt flag
    {
        bool VBlank;
        bool LCD;
        bool Timer;
        bool Serial;
        bool Joypad;
    };
    struct LCDC_str {
        bool BG_Window_enable_priority;
        bool OBJ_enable;
        bool OBJ_size;
        bool BG_tile_map;
        bool BG_Window_tiles;
        bool Window_enable;
        bool Window_tile_map;
        bool LCD_PPU_enable;
    };
    
    IE_str IE{0,0,0,0};
    IF_str IF{0,0,0,0};

    LCDC_str LCDC{1,0,0,1,1,0,1,1};
    uint8_t BG_Palette[4];    // Palette de fond (Background)
    uint8_t OBJ_Palette_0[4]; // Sprite Palette 0 (0BP0)
    uint8_t OBJ_Palette_1[4]; // Sprite Palette 1 (0BP1)


    uint8_t Memory[0xFFFF + 1];
    uint8_t OAM[0xFE9F - 0xFE00 + 1];
    uint8_t VRAM[0x2000 + 1];
    char RomName[16];  
    char licensee_code[1];
    
    std::string toHexString(uint8_t value);
    std::string toHexString(uint16_t value);
    std::string toHexString(int8_t value);

    void Init(const char *path);
    void LoadGame(const char* path);
    void AddOpcodeEntry(uint16_t address, uint8_t id, const std::string& description, uint16_t opcode);
    void _cpu();

};
