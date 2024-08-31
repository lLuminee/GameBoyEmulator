#ifndef OPCODEHISTORY_H
#define OPCODEHISTORY_H

#include <vector>
#include <tuple>
#include <string>
#include <cstdint> // Ajoutez ceci pour les types uint16_t et uint8_t
#include "cpu.h"

class OpcodeHistory {
public:
    OpcodeHistory(Cpu& cpu);
    void Render(Cpu& cpu);

private:
    const int maxHistorySize = 18; // Taille maximale de l'historique
    Cpu& cpu;

};

#endif // OPCODEHISTORY_H
