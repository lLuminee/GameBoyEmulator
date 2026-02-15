#include "../include/ppu.h"
#include "../include/cpu.h"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <SFML/Graphics.hpp>

// 0xFE00 Debut de OAM
// 0xFE9F Fin de OAM

void grid::Init(uint8_t* Memory, Cpu* cpu)
{
    std::cout << "Initializing grid" << std::endl;
    memset(Main_grid, 0, sizeof(Main_grid));
    memset(Tile_Block, 0, sizeof(Tile_Block));
    memset(OAM_Palette, 0, sizeof(OAM_Palette));
    CalculateTile(Memory, cpu);

}

void print(sf::RenderWindow* Window,int x, int y, int SizePixel, int color, Cpu* cpu, int palette) {
    sf::RectangleShape rectangle(sf::Vector2f(SizePixel, SizePixel));
    rectangle.setPosition(sf::Vector2f(static_cast<float>(x * SizePixel), static_cast<float>(y * SizePixel)));

    uint8_t paletteColor[4];

    if (palette == 0){memcpy(paletteColor, cpu->BG_Palette,    sizeof(paletteColor));}
    if (palette == 1){memcpy(paletteColor, cpu->OBJ_Palette_0, sizeof(paletteColor));}
    if (palette == 2){memcpy(paletteColor, cpu->OBJ_Palette_1, sizeof(paletteColor));}
    
    switch (paletteColor[color]) {
        case 0:
            rectangle.setFillColor(sf::Color(255, 255, 255));
            break; 
        case 1:
            rectangle.setFillColor(sf::Color(192, 192, 192));
            break;
        case 2:
            rectangle.setFillColor(sf::Color(96, 96, 96));
            break;
        case 3:
            rectangle.setFillColor(sf::Color(0, 0, 0));
            break;
        default:
            rectangle.setFillColor(sf::Color(125,125,125));
            break;
    }
    Window->draw(rectangle);
}

void grid::Render_OAM(int block_id, int x, int y, uint8_t flags, Cpu* cpu) {
    // Extraction des flags individuels
    bool Priority = flags & 0x80;        // Sprite priority over background
    bool y_flip = flags & 0x40;          // Vertical flip
    bool x_flip = flags & 0x20;          // Horizontal flip
    bool DMG_palette = flags & 0x10;     // DMG palette selection

    uint8_t Palette = DMG_palette ? 1 : 0;
    Palette += 1;

    // Parcours de chaque ligne et chaque pixel du bloc
    for (int line = 0; line < 8; ++line) {
        for (int pixel = 0; pixel < 8; ++pixel) {
            // Détermination des positions réelles en fonction des retournements
            int actual_line = y_flip ? 7 - line : line;
            int actual_pixel = x_flip ? 7 - pixel : pixel;

            // Calcul de l'index dans le bloc de tiles
            int pixel_index = actual_line * 8 + actual_pixel;
            uint8_t pixel_value = Tile_Block[block_id][pixel_index];

            if (!Priority || Main_grid[x + pixel][y + line] == 0) {
            // Soit priorité = 0 (sprite devant tout),
            // soit le fond est transparent (index 0),
            // donc on affiche le sprite
            Main_grid[x + pixel][y + line] = pixel_value;
            OAM_Palette[x + pixel][y + line] = Palette;
            }
        }
    }
}


void grid::Render(Cpu* cpu, sf::RenderWindow* Window) {
    int palette;
    for (int x = 0; x < 160; ++x) {
        for (int y = 0; y < 144; ++y) {
            
            if (OAM_Palette[x][y] == 1 || OAM_Palette[x][y] == 2 ){
                palette = OAM_Palette[x][y];
            } else {
                palette = 0;
            }
            
            print(Window,x, y, 5, Main_grid[x][y], cpu, palette);
            

        }
    }
}



void grid::Tile_Block_Render(uint8_t* Memory, Cpu* cpu, sf::RenderWindow* Window) {

    const int SizeTilePixel = 2;  // Taille d'un pixel en pixels

    for (int block_index = 0; block_index < 382; ++block_index) {
        for (int line = 0; line < 8; ++line) {
            for (int pixel = 0; pixel < 8; ++pixel) {
                uint8_t colorID = Tile_Block[block_index][line * 8 + pixel];  // 64 pixels par block
                print(Window,(block_index % 16) * 8 + pixel, (block_index / 16) * 8  + line, SizeTilePixel, colorID, cpu, 0);
            }
        }
    }
}


void grid::CalculateTile(uint8_t* Memory, Cpu* cpu) {
    memcpy(cpu->VRAM, Memory + 0x8000, sizeof(cpu->VRAM));
    uint8_t offset_tile_map;
    if (cpu->LCDC.BG_Window_tiles == 1) {offset_tile_map = 128;}; 
    if (cpu->LCDC.BG_Window_tiles == 0) {offset_tile_map = 0;}; 
    // ------- TEST 
    offset_tile_map = 0;
    // -------- TEST

    // Calcul du tile block
    for (int block_index = 0; block_index < 382; ++block_index) {
        for (int line = 0; line < 8; ++line) {
            uint8_t byte1 = cpu->VRAM[block_index * 16 + line * 2];
            uint8_t byte2 = cpu->VRAM[block_index * 16 + line * 2 + 1];

            for (int pixel = 0; pixel < 8; ++pixel) {
                uint8_t colorBit0 = (byte1 >> (7 - pixel)) & 0x01;
                uint8_t colorBit1 = (byte2 >> (7 - pixel)) & 0x01;
                uint8_t colorID = (colorBit1 << 1) | colorBit0;

                Tile_Block[block_index][line * 8 + pixel] = colorID;  // 64 pixels par block, donc line * 8 + pixel
            }
        }
    }

    // Calcul de la tile map 1 
    for (int index = 0; index < 0x400; ++index) {
        int x = 0;
        int y = 0;

        int tileID = cpu->VRAM[index + 0x1800];

        // Calcul des coordonnées (x, y)
        x = (index % 32) * 8;
        y = (index / 32) * 8;

        // Copier les pixels de la tuile dans la Tile_Map_1
        for (int pixel = 0; pixel < 64; ++pixel) {
            int col = x + (pixel % 8);
            int row = y + (pixel / 8);
            uint8_t value = Tile_Block[tileID + offset_tile_map][pixel];
            Tile_Map_1[col][row] = value;
        }
    }
    
    // Calcul de la tile map 2
    for (int index = 0; index < 0x400; ++index) {
        int x = 0;
        int y = 0;
        int tileID = cpu->VRAM[index + 0x1C00];

        // Calcul des coordonnées (x, y)
        x = (index % 32) * 8;
        y = (index / 32) * 8;

        // Copier les pixels de la tuile dans la Tile_Map_1
        for (int pixel = 0; pixel < 64; ++pixel) {
            int col = x + (pixel % 8);
            int row = y + (pixel / 8);
            uint8_t value = Tile_Block[tileID + offset_tile_map][pixel];
            Tile_Map_2[col %248 ][row % 248] = value;
        }
    }

    // Calcul du framebuffer
    memset(&Main_grid[0][0], 0, sizeof(Main_grid));

    // Calculé la fenêtre visible
    for (int y = 0; y < 144; y++) {  // Hauteur de la fenêtre visible
        for (int x = 0; x < 160; x++) {  // Largeur de la fenêtre visible
            
            // Calculer les coordonnées du tilemap
            int tile_x = (cpu->SCX + x) % 256;  // Modulo pour le wrapping horizontal
            int tile_y = (cpu->SCY + y) % 256;  // Modulo pour le wrapping vertical
                        
            // Afficher le pixel à l'écran
            if (cpu->LCDC.BG_tile_map == 0) {
                Main_grid[x][y] = Tile_Map_1[tile_x][tile_y];

            } else{
                Main_grid[x][y] = Tile_Map_2[tile_x][tile_y];

            }
        }
    }

    
    // Calcule de OAM
    uint8_t NubmerOfPrintOnMemeLigne = 0;
    uint8_t Last_Y = 0;
    for (int i = 0; i < 160; i += 4) {
        int y = cpu->OAM[i] - 16;      // Soustraire 16 pour aligner le sprite
        int x = cpu->OAM[i + 1] - 8;   // Soustraire 8 pour aligner le sprite
        int id = cpu->OAM[i + 2];
        int flag = cpu->OAM[i + 3];    

        if (Last_Y == y) {NubmerOfPrintOnMemeLigne++;} else {NubmerOfPrintOnMemeLigne = 0; Last_Y = y;}

        if (NubmerOfPrintOnMemeLigne < 10) {
            
            Render_OAM(id, x, y, flag, cpu);

        }
        
    }

}


void grid::Tile_Map_Render(uint8_t* Memory, Cpu* cpu, sf::RenderWindow* Window) {

    for (int x = 0; x < 31 * 8; ++x) {
        for (int y = 0; y < 31 * 8; ++y) {
            uint8_t color = Tile_Map_1[x][y];
            print(Window,x , y + 200, 2, color, cpu, 0);

        }
    }


    for (int x = 0; x < 31 * 8; ++x) {
        for (int y = 0; y < 31 * 8; ++y) {
            uint8_t color = Tile_Map_2[x][y];
            print(Window,x +248 , y + 200, 2, color, cpu, 0);

        }
    }


}

