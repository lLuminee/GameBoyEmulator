#ifndef CARTRIDGESCREEN_H
#define CARTRIDGESCREEN_H

#include <imgui.h>
#include "cpu.h"

class CartridgeScreen {
public:
    CartridgeScreen(Cpu& cpu);
    void Render();

private:
    Cpu& cpu;
};

#endif // CARTRIDGESCREEN_H
