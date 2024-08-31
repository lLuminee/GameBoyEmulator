#ifndef DEBUGSCREEN_H
#define DEBUGSCREEN_H

#include <imgui.h>
#include "cpu.h"

class DebugScreen {
public:
    DebugScreen(Cpu& cpu);
    void Render();

private:
    Cpu& cpu;
};

#endif // DEBUGSCREEN_H
