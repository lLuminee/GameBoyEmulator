#include "../../include/CartridgeScreen.h"
#include "../../include/cpu.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <imgui.h>

CartridgeScreen::CartridgeScreen(Cpu& cpu) : cpu(cpu) {}

void CartridgeScreen::Render() {
    std::string Rom_Name = "Rom Name: " + std::string(cpu.RomName);
    ImGui::Text(Rom_Name.c_str());
    std::string licensee_code = "Licensee code: " + std::string(cpu.licensee_code);
    ImGui::Text(licensee_code.c_str());
    ImGui::Checkbox("Show VRAM", &cpu.ShowVram);

    if(cpu.ShowVram) {
        if (!cpu.VRAM_Window.isOpen()) {
        cpu.VRAM_Window.create(sf::VideoMode({800, 800}), "VRAM");
        }
    }else {
        cpu.VRAM_Window.close();
    }

}
