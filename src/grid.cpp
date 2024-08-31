#include "../include/grid.h"
#include "../include/cpu.h"
#include <iostream>
#include <SDL.h>
#include <cstring>
#include <iomanip>
#include <sstream>

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

void print(SDL_Renderer* renderer, int x, int y, int SizePixel, int color, Cpu* cpu, bool palette) {
    SDL_Rect rect = {x * SizePixel, y * SizePixel, SizePixel, SizePixel};

    uint8_t paletteColor;

    if (palette == 0) {paletteColor = cpu->OBJ_Palette_0[color];}
    else {paletteColor = cpu->OBJ_Palette_1[color];}
    
    switch (paletteColor) {
        case 0:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); // White
            break;
        case 1:
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, SDL_ALPHA_OPAQUE); // Light Gray
            break;
        case 2:
            SDL_SetRenderDrawColor(renderer, 96, 96, 96, SDL_ALPHA_OPAQUE); // Dark Gray
            break;
        case 3:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Black
            break;
        default:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir par défaut
            break;
    }
    SDL_RenderFillRect(renderer, &rect);
}    

void grid::Render_OAM(int block_id, int x, int y, uint8_t flags, Cpu* cpu) {
    // Extraction des flags individuels
    bool Priority = flags & 0b10000000;
    bool y_flip = flags & 0b01000000;
    bool x_flip = flags & 0b00100000;
    bool DMG_palette = flags & 0b00010000;
    bool Bank = flags & 0b00001000;

    // Ajustement du block_id si le Bank est activé
    if (Bank) {
        block_id += 255;
    }

    // Parcours de chaque ligne et chaque pixel du bloc
    for (int line = 0; line < 8; ++line) {
        for (int pixel = 0; pixel < 8; ++pixel) {
            // Détermination des indices réels en fonction des flips
            int actual_line = y_flip ? 7 - line : line;
            int actual_pixel = x_flip ? 7 - pixel : pixel;

            // Calcul de l'index du pixel dans le bloc de tuiles
            int pixel_index = actual_line * 8 + actual_pixel;

            // Récupération de la valeur du pixel dans le bloc de tuiles
            uint8_t pixel_value = Tile_Block[block_id][pixel_index];

            // Application de la palette DMG si nécessaire
            if (DMG_palette && Main_grid[x + pixel][y + line] == 0) {
                OAM_Palette[x + pixel][y + line] = 1;
            } else {
                OAM_Palette[x + pixel][y + line] = 0;
            }

            // Rendu dans la grille principale en fonction de la priorité
            if (Priority) {
                Main_grid[x + pixel][y + line] = pixel_value;
            } else {
                if (Main_grid[x + pixel][y + line] != 0) {
                    Main_grid[x + pixel][y + line] |= pixel_value;

                }
            }
        }
    }
}


void grid::Render(SDL_Renderer* renderer, Cpu* cpu) {
    bool palette;
    for (int x = 0; x < 160; ++x) {
        for (int y = 0; y < 144; ++y) {
            if (OAM_Palette[x][y] == 1) {palette = true;} else {palette = false;}
            print(renderer, x, y, 5, Main_grid[x][y], cpu, palette);
        }
    }
}



void grid::Tile_Block_Render(SDL_Renderer* renderer, uint8_t* Memory, Cpu* cpu) {

    const int SizeTilePixel = 2;  // Taille d'un pixel en pixels

    for (int block_index = 0; block_index < 382; ++block_index) {
        for (int line = 0; line < 8; ++line) {
            for (int pixel = 0; pixel < 8; ++pixel) {
                uint8_t colorID = Tile_Block[block_index][line * 8 + pixel];  // 64 pixels par block
                print(renderer, (block_index % 16) * 8 + pixel, (block_index / 16) * 8  + line, SizeTilePixel, colorID, cpu, 0);
            }
        }
    }
}


void grid::CalculateTile(uint8_t* Memory, Cpu* cpu) {
    memcpy(cpu->VRAM, Memory + 0x8000, sizeof(cpu->VRAM));

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
            uint8_t value = Tile_Block[tileID][pixel];
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
            uint8_t value = Tile_Block[tileID][pixel];
            Tile_Map_2[col %248 ][row % 248] = value;
        }
    }

    // Calcul du framebuffer
    std::fill(&Main_grid[0][0], &Main_grid[0][0] + sizeof(Main_grid), 0);

    // Affichage de la fenêtre visible
    for (int y = 0; y < 144; y++) {  // Hauteur de la fenêtre visible
        for (int x = 0; x < 160; x++) {  // Largeur de la fenêtre visible
            
            // Calculer les coordonnées du tilemap
            int tile_x = (cpu->SCX + x) % 256;  // Modulo pour le wrapping horizontal
            int tile_y = (cpu->SCY + y) % 256;  // Modulo pour le wrapping vertical
                        
            // Afficher le pixel à l'écran
            if (cpu->ActiveTileMap == 0) {
                Main_grid[x][y] = Tile_Map_1[tile_x][tile_y];

            } else{
                Main_grid[x][y] = Tile_Map_2[tile_x][tile_y];

            }
        }
    }

    uint8_t NubmerOfPrintOnMemeLigne = 0;
    uint8_t Last_Y = 0;
    for (int i = 0; i < 160; i += 4) {
        int y = cpu->OAM[i] - 16;      // Soustraire 16 pour aligner le sprite
        int x = cpu->OAM[i + 1] - 8;   // Soustraire 8 pour aligner le sprite
        int id = cpu->OAM[i + 2];
        int flag = cpu->OAM[i + 3];    // Les attributs du sprite (non utilisés ici)

        if (Last_Y == y) {NubmerOfPrintOnMemeLigne++;} else {NubmerOfPrintOnMemeLigne = 0; Last_Y = y;}

        if (NubmerOfPrintOnMemeLigne < 10) {
            Render_OAM(id, x, y, flag, cpu);
        }
        
    }


}


void grid::Tile_Map_Render(SDL_Renderer* renderer, uint8_t* Memory, Cpu* cpu) {

    for (int x = 0; x < 31 * 8; ++x) {
        for (int y = 0; y < 31 * 8; ++y) {
            uint8_t color = Tile_Map_1[x][y];
            print(renderer, x , y + 200, 2, color, cpu, 0);

        }
    }


    for (int x = 0; x < 31 * 8; ++x) {
        for (int y = 0; y < 31 * 8; ++y) {
            uint8_t color = Tile_Map_2[x][y];
            print(renderer, x +248 , y + 200, 2, color, cpu, 0);

        }
    }


}

