#ifndef MEMORYSCREEN_H
#define MEMORYSCREEN_H

#include <imgui.h>
#include <imgui_memory_editor.h>
#include "cpu.h"

class MemoryScreen {
public:
    MemoryScreen(Cpu& cpu);
    void Render();

private:
    Cpu& cpu;

};

#endif // MEMORYSCREEN_H
