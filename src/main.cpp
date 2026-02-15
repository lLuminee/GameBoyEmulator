#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui.h>
#include <imgui_memory_editor.h>
#include <imgui-SFML.h>
#include <nfd.h>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex> 

#include "../include/cpu.h"
#include "../include/ppu.h"
#include "../include/opcode.h"
#include "../include/MainScreen.h"
#include "../include/OpcodeHistory.h"

#define WIDTH 1600 
#define HEIGHT 720 

const float FPS = 60.0f;
const float FRAME_DURATION = 1000.0f / FPS;
const int CYCLES_PER_FRAME = 70224;  // Approximation pour une frame (~59.7 Hz)
const int SCANLINE_CYCLES = 456;     // Cycles d'horloge par ligne de scanline
std::atomic<bool> running(true);  // Indicateur de fonctionnement global
std::mutex cpuMutex;
bool MainRunning = true;
sf::Clock deltaClock;

void CheckInterrupts (Cpu &cpu) {
    /*Interrupt enable*/
    for (int bit_index = 0; bit_index < 8; bit_index++) {
        // Extraire le bit correspondant
        uint8_t bit = (cpu.Memory[0xFFFF] >> (8 - bit_index)) & 1;

        // Assigner le bit à la structure IE (supposant que IE a des membres comme bit0, bit1, etc.)
        switch (8 - bit_index) {
            case 0:
                cpu.IE.VBlank = bit;
                break;
            case 1:
                cpu.IE.LCD = bit;
                break;
            case 2:
                cpu.IE.Timer = bit;
                break;
            case 3:
                cpu.IE.Serial = bit;
                break;
            case 4:
                cpu.IE.Joypad = bit;
                break;
        }
    }
    /*Interrupt flag*/
    if (cpu.IME == 1) {
        if (cpu.LY == cpu.LYC){
            cpu.IF.LCD = 1;
        }
    }
    /*Verify*/
    if (cpu.IME == 1) {
        if (cpu.IE.VBlank == 1 && cpu.IF.VBlank == 1) {
            cpu.stack.push_back(cpu.pc);
            cpu.pc = 0x0040;
            std::cout << "VBlank interrupt" << std::endl;
            cpu.IsHalt = 0;
            cpu.IME = 0;
            cpu.IF.VBlank = 0;  // Remise à zéro de l'interruption VBlank traitée
        }
        if (cpu.IE.LCD == 1 && cpu.IF.LCD == 1) {
            cpu.stack.push_back(cpu.pc);
            cpu.pc = 0x0048;
            std::cout << "LCD interrupt" << std::endl;
            cpu.IsHalt = 0;
            cpu.IME = 0;
            cpu.IF.LCD = 0;  // Remise à zéro de l'interruption VBlank traitée
        }
    }
    std::cout << "\n\n\n\n\n\n";
    if (cpu.IME) {std::cout << "IME = TRUE" <<std::endl;}
    if (cpu.IE.LCD) {std::cout << "IE.LCD = TRUE" <<std::endl;}
    if (cpu.IF.LCD) {std::cout << "IF.LCD = TRUE" <<std::endl;}

}

void EmulationThread(Cpu &cpu, Opcode &op, grid &grid) {
    (void) grid;
    int cycleCount = 0;
    while (running) {
        
        if (cpu.Step || cpu.isStep == false) {
            std::cout << "Cycle : " << cycleCount << std::endl;
            {
                std::lock_guard<std::mutex> lock(cpuMutex);
                cycleCount += cpu.OpCycle;  // Accumuler les cycles effectués par chaque opcode
                op.OpcodeStep(cpu.isStep, op, cpu);
                cpu._cpu();
                CheckInterrupts(cpu);
            }
        }
        
        if (cycleCount >= SCANLINE_CYCLES) {
            {
                std::lock_guard<std::mutex> lock(cpuMutex);
                cpu.Memory[0xFF44]++;
                if (cpu.Memory[0xFF44] > 153) {
                    cpu.Memory[0xFF44] = 0;
                }
                if (cpu.Memory[0xFF44] >= 144) {
                    cpu.Memory[0xFF0F] |= 0x01;
                }
                cycleCount -= SCANLINE_CYCLES;
            }
        }

        if (cycleCount >= CYCLES_PER_FRAME) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(FRAME_DURATION)));
            cycleCount = 0;
        }
        if (cpu.Main_Window.isOpen() == false) {
            MainRunning = false;
            running = false;
            break;
        }

    }
}

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;
    std::cout << "Starting GameLumine \n\n";
    grid grid;
    Cpu cpu;
    Opcode op;

    cpu.Init("C:/Users/Dorian/OneDrive/Dev/C++/GB-Emulateur/Gbv0.7/GB-Emulateur/out/rom/dmg-acid2.gb");
    grid.Init(cpu.Memory, &cpu);

    cpu.Main_Window.create(sf::VideoMode({1600, 720}), "ImGui + SFML = <3");
    cpu.Main_Window.setFramerateLimit(60);

    /*
    cpu.VRAM_Window.create(sf::VideoMode({800, 800}), "VRAM");
    cpu.VRAM_Window.setFramerateLimit(60);
    */
    if (!ImGui::SFML::Init(cpu.Main_Window)) {
    std::cerr << "Failed to initialize ImGui for Main_Window!" << std::endl;
    return -1;
    }


    
    std::thread emulationThread(EmulationThread, std::ref(cpu), std::ref(op), std::ref(grid));
    
    while (cpu.Main_Window.isOpen() && MainRunning) {
        while (const auto event = cpu.Main_Window.pollEvent()) {
            ImGui::SFML::ProcessEvent(cpu.Main_Window, *event);     

            if (event->is<sf::Event::Closed>()) {
                cpu.Main_Window.close();
            }
        }

        if (cpu.IsReset) {
            cpu.Init(cpu.RomPath);
            grid.Init(cpu.Memory, &cpu);
            cpu.IsReset = false;
        }
        ImGui::SFML::Update(cpu.Main_Window, deltaClock.restart());
        MainScreen mainScreen(cpu);
        
        cpu.Main_Window.clear();
        mainScreen.Render();
        
        ImGui::SFML::Render(cpu.Main_Window); 
        grid.CalculateTile(cpu.Memory, &cpu);
        grid.Render(&cpu, &cpu.Main_Window);
        cpu.Main_Window.display();

        if (cpu.ShowVram) {
            cpu.VRAM_Window.clear();
            grid.Tile_Block_Render(cpu.Memory, &cpu, &cpu.VRAM_Window);
            grid.Tile_Map_Render(cpu.Memory, &cpu, &cpu.VRAM_Window);
            cpu.VRAM_Window.display();
        }

        
        
    }
    ImGui::SFML::Shutdown();
    emulationThread.join();

    return 0;
}
