#pragma once
#include "../include/cpu.h"
#include <SFML/Graphics.hpp>

class grid
{
public:
    void Init(uint8_t* Memory, Cpu* cpu);
    void Render(Cpu* cpu, sf::RenderWindow* Window);
    void Tile_Block_Render(uint8_t* Memory, Cpu* cpu, sf::RenderWindow* Window);
    void Tile_Map_Render(uint8_t* Memory, Cpu* cpu, sf::RenderWindow* Window);
    
    void PlacePixel(int x, int y, int color);

    void CalculateTile(uint8_t* Memory, Cpu* cpu);


private:
    int Main_Number_Colons = 160;
    int Main_Number_Rows = 144;

    int Tile_NumberColons = 16 * 8 /* 16 Tile of 16bytes*/;
    int Tile_NumberRows = 24 * 8 /* 24 Tile of 16bytes*/;

    int  Size_pixel = 5;
    int  Size_tile = 2;

    //uint8_t Tile_grid[16 * 8][24 * 8];
    uint8_t mem[384];
    uint8_t Main_grid[160][144]; // Main grid
    uint8_t OAM_Palette[160][144];
    uint8_t Tile_Block[382][64];    // Tile Block
    uint8_t Tile_Map_1[31 * 8][31 * 8];
    uint8_t Tile_Map_2[31 * 8][31 * 8];
    // Tile Map

    void Render_OAM(int block_id, int x, int y,uint8_t flage , Cpu* cpu);


};

