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
#include "../include/grid.h"
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

void EmulationThread(Cpu &cpu, Opcode &op) {
    int cycleCount = 0;
    while (running) {
        std::lock_guard<std::mutex> lock(cpuMutex);
        // Exécuter les instructions du CPU
        if (cpu.Step || cpu.isStep == false) {
            cycleCount += cpu.OpCycle;  // Accumuler les cycles effectués par chaque opcode
        }
        
        op.OpcodeStep(cpu.isStep, op, cpu);
        cpu._cpu();
        

        if (cycleCount >= SCANLINE_CYCLES ) {
            cpu.Memory[0xFF44]++;
            if (cpu.Memory[0xFF44] > 153) {  // Après 154 lignes, revenir à 0
                cpu.Memory[0xFF44] = 0;
            }
            cycleCount -= SCANLINE_CYCLES;  // Réinitialiser le compteur de cycles
        }

        // Synchronisation en fonction des cycles CPU
        if (cycleCount >= CYCLES_PER_FRAME) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(FRAME_DURATION)));
            cycleCount = 0;  // Réinitialiser le compteur de cycles
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

    if (!SDL_INIT_EVERYTHING) { // Initialize SDL2
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* windo = SDL_CreateWindow("GameLumine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WITH, HEIGHT, SDL_WINDOW_SHOWN);
    if (windo == nullptr){
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(windo, -1, 0);
    if (renderer == nullptr){
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return 1;
    }   

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    ImGui_ImplSDL2_InitForSDLRenderer(windo, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    
    std::thread emulationThread(EmulationThread, std::ref(cpu), std::ref(op));
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
                if (event.window.windowID == SDL_GetWindowID(windo)) {
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
        
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        mainScreen.Render();
        ImGui::Render();

        grid.CalculateTile(cpu.Memory, &cpu);
        grid.Render(renderer, &cpu);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);

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
            std::cout << "Frame overrun by " << elapsed.count() - FRAME_DURATION << " ms\n";
        }
        
    }


    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(windo);
    SDL_DestroyRenderer(cpu.VRAM_Renderer);
    SDL_DestroyWindow(cpu.VRAM_Windo);
    SDL_Quit();

    return 0;
}   
