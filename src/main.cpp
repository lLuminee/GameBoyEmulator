#include <iostream>
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_memory_editor.h>
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

#define WITH 1600 
#define HEIGHT 720 

const float FPS = 60.0f;
const float FRAME_DURATION = 1000.0f / FPS;
const int CYCLES_PER_FRAME = 70224;  // Approximation pour une frame (~59.7 Hz)
const int SCANLINE_CYCLES = 456;     // Cycles d'horloge par ligne de scanline
std::atomic<bool> running(true);  // Indicateur de fonctionnement global
std::mutex cpuMutex;
bool MainRunning = true;


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
    int cycleCount = 0;
    while (running) {

        
        grid.CalculateTile(cpu.Memory, &cpu);
        
        if (cpu.Step || cpu.isStep == false) {
            std::cout << "Cycle : " << cycleCount << std::endl;
            std::lock_guard<std::mutex> lock(cpuMutex);
            cycleCount += cpu.OpCycle;  // Accumuler les cycles effectués par chaque opcode
            op.OpcodeStep(cpu.isStep, op, cpu);
            cpu._cpu();
            CheckInterrupts(cpu);
        }
        
        if (cycleCount >= SCANLINE_CYCLES) {
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

        if (cycleCount >= CYCLES_PER_FRAME) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(FRAME_DURATION)));
            cycleCount = 0;
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

    cpu.Init("rom/dmg-acid2.gb");
    grid.Init(cpu.Memory, &cpu);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    cpu.Main_Windo = SDL_CreateWindow("GameLumine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WITH, HEIGHT, SDL_WINDOW_SHOWN);
    if (cpu.Main_Windo == nullptr){
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    cpu.FrameBuffer = SDL_CreateRenderer(cpu.Main_Windo, -1, 0);
    if (cpu.FrameBuffer == nullptr){
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return 1;
    }   

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    ImGui_ImplSDL2_InitForSDLRenderer(cpu.Main_Windo, cpu.FrameBuffer);
    ImGui_ImplSDLRenderer2_Init(cpu.FrameBuffer);

    
    std::thread emulationThread(EmulationThread, std::ref(cpu), std::ref(op), std::ref(grid));
    auto previous = std::chrono::high_resolution_clock::now();
    
    while (MainRunning) {
        auto current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> elapsed = current - previous;
        previous = current;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                MainRunning = false;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                if (event.window.windowID == SDL_GetWindowID(cpu.Main_Windo)) {
                    MainRunning = false;
                } else if (event.window.windowID == SDL_GetWindowID(cpu.VRAM_Windo)) {
                    cpu.ShowVram = false;
                    SDL_DestroyRenderer(cpu.VRAM_Renderer);
                    SDL_DestroyWindow(cpu.VRAM_Windo);
                    cpu.VRAM_Renderer = nullptr;
                    cpu.VRAM_Windo = nullptr;
                }
            }
            
            ImGui_ImplSDL2_ProcessEvent(&event);
        }
        if (cpu.IsReset) {
            cpu.Init(cpu.RomPath);
            grid.Init(cpu.Memory, &cpu);
            cpu.IsReset = false;
        }

        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui::NewFrame();

        MainScreen mainScreen(cpu);
        
        SDL_SetRenderDrawColor(cpu.FrameBuffer, 0,0,0,255);
        SDL_RenderClear(cpu.FrameBuffer);

        mainScreen.Render();
        ImGui::Render();

    
        grid.Render(cpu.FrameBuffer, &cpu);


        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), cpu.FrameBuffer);

        SDL_RenderPresent(cpu.FrameBuffer);

        if (cpu.ShowVram == true) {
            SDL_SetRenderDrawColor(cpu.VRAM_Renderer, 0, 0, 0, 255);
            SDL_RenderClear(cpu.VRAM_Renderer);
            grid.Tile_Block_Render(cpu.VRAM_Renderer, cpu.Memory, &cpu);
            grid.Tile_Map_Render(cpu.VRAM_Renderer, cpu.Memory, &cpu);
            SDL_RenderPresent(cpu.VRAM_Renderer);
        }
        
        if (elapsed.count() < FRAME_DURATION) {
            SDL_Delay(static_cast<Uint32>(FRAME_DURATION - elapsed.count()));
        } else {
            //std::cout << "Frame overrun by " << elapsed.count() - FRAME_DURATION << " ms\n";
        }
        
    }


    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(cpu.FrameBuffer);
    SDL_DestroyWindow(cpu.Main_Windo);
    SDL_DestroyRenderer(cpu.VRAM_Renderer);
    SDL_DestroyWindow(cpu.VRAM_Windo);
    SDL_Quit();

    return 0;
}   
