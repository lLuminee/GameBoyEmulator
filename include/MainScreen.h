#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <vector>
#include "Cpu.h"
#include "DebugScreen.h"
#include "MemoryScreen.h"
#include "CartridgeScreen.h"
#include "OpcodeHistory.h"

class MainScreen {
public:
    MainScreen(Cpu& cpu);

    void Render();

private:
    Cpu& cpu;
    DebugScreen debugScreen;
    MemoryScreen memoryScreen;
    CartridgeScreen cartridgeScreen;
    OpcodeHistory opcodeHistory;
};

#endif // MAINSCREEN_H
