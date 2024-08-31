#include "../../include/CartridgeScreen.h"
#include "../../include/cpu.h"
#include <iostream>
#include <SDL.h>

#include <imgui.h>

CartridgeScreen::CartridgeScreen(Cpu& cpu) : cpu(cpu) {}

void CartridgeScreen::Render() {
    std::string Rom_Name = "Rom Name: " + std::string(cpu.RomName);
    ImGui::Text(Rom_Name.c_str());
    std::string licensee_code = "Licensee code: " + std::string(cpu.licensee_code);
    ImGui::Text(licensee_code.c_str());
    ImGui::Checkbox("Show VRAM", &cpu.ShowVram);

    if (cpu.ShowVram && cpu.VRAM_Windo == nullptr && cpu.VRAM_Renderer == nullptr) {
        cpu.VRAM_Windo = SDL_CreateWindow("VRAM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1500, 1000, SDL_WINDOW_SHOWN);
        if (cpu.VRAM_Windo == nullptr) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            exit(1);
        }

        std::cout << "VRAM window created" << std::endl;

        cpu.VRAM_Renderer = SDL_CreateRenderer(cpu.VRAM_Windo, -1, 0);
        if (cpu.VRAM_Renderer == nullptr) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            exit(1);
        }
    } 
    
    if(cpu.ShowVram == false&& cpu.VRAM_Windo != nullptr && cpu.VRAM_Renderer != nullptr) {
        cpu.VRAM_Windo = nullptr;
        cpu.VRAM_Renderer = nullptr;
    }
}
