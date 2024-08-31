#include <iostream>
#include <cstring>
#include "../include/cpu.h"
#include <iostream>
#include <SDL.h>
#include <iomanip>
#include <sstream>
#include <fstream>

void Cpu::Init(const char *path) {
    std::cout << "Initializing CPU" << std::endl;

    RomPath = path;

    memset(&Memory, 0, sizeof(Memory));

    pc = 0x0100;
    sp = 0x0000;
    last_pc = 0x0100;

    id = 0x00;
    NextId = 0x00;

    AF = 0x0000; // Accumulator
    BC = 0x0000; // B HI C low
    DE = 0x0000; // D HI E low
    HL = 0x0000; // H HI L low

    z = 0x00; // zero flag
    n = 0x00; // subtract flag
    h = 0x00; // half carry flag
    c = 0x00; // carry flag

    RomName[16] = '\0';
    licensee_code[1] = 0;

    LoadGame(path);
    std::cout << "Rom name: " <<std::string(RomName) << std::endl;
    Memory[0xFF40] = 0x91;
    
}

void Cpu::Get_licensee_code() {
    /* 0144–0145 — New licensee code */
    for (int i = 0; i <= 1; i++) {
        licensee_code[i] = static_cast<char>(Memory[0x144 + i]);
    }

/* License code:
00	None
01	Nintendo Research & Development 1
08	Capcom
13	EA (Electronic Arts)
18	Hudson Soft
19	B-AI
20	KSS
22	Planning Office WADA
24	PCM Complete
25	San-X
28	Kemco
29	SETA Corporation
30	Viacom
31	Nintendo
32	Bandai
33	Ocean Software/Acclaim Entertainment
34	Konami
35	HectorSoft
37	Taito
38	Hudson Soft
39	Banpresto
41	Ubi Soft1
42	Atlus
44	Malibu Interactive
46	Angel
47	Bullet-Proof Software2
49	Irem
50	Absolute
51	Acclaim Entertainment
52	Activision
53	Sammy USA Corporation
54	Konami
55	Hi Tech Expressions
56	LJN
57	Matchbox
58	Mattel
59	Milton Bradley Company
60	Titus Interactive
61	Virgin Games Ltd.3
64	Lucasfilm Games4
67	Ocean Software
69	EA (Electronic Arts)
70	Infogrames5
71	Interplay Entertainment
72	Broderbund
73	Sculptured Software6
75	The Sales Curve Limited7
78	THQ
79	Accolade
80	Misawa Entertainment
83	lozc
86	Tokuma Shoten
87	Tsukuda Original
91	Chunsoft Co.8
92	Video System
93	Ocean Software/Acclaim Entertainment
95	Varie
96	Yonezawa/s’pal
97	Kaneko
99	Pack-In-Video
9H	Bottom Up
A4	Konami (Yu-Gi-Oh!)
BL	MTO
DK	Kodansha

*/
std::string code = std::string(licensee_code);


    /* License code */
    if (strcmp(code.c_str(), "00") == 0) {
        std::cout << "None" << std::endl;
    } else if (strcmp(code.c_str(), "01") == 0) {
        std::cout << "Nintendo Research & Development 1" << std::endl;
    } else if (strcmp(code.c_str(), "08") == 0) {
        std::cout << "Capcom" << std::endl;
    } else {
        std::cout << "Unknown Code: " << code <<std::endl;
    }
}


void Cpu::GetRomName() {
    /* 0134-0143 — Title */

    for (int i = 0; i <= 0x0143 - 0x0134; i++) {
        RomName[i] = static_cast<char>(Memory[0x134 + i]);
    }

    // Null terminate the string
    RomName[15] = '\0';


}

void Cpu::LoadGame(const char* path) {
    FILE *rom = fopen(path, "rb");
    if (!rom) {
        std::cout << "Failed to open ROM: " << path << std::endl;
        return;
    }
    fread(&Memory, 0xffff, 1, rom);
    fclose(rom);
    GetRomName();
    Get_licensee_code();

}



std::string Cpu::toHexString(uint8_t value) {
    std::ostringstream oss;
    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
    return oss.str();

}

std::string Cpu::toHexString(uint16_t value) {
    std::ostringstream oss;
    oss << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(value);
    return oss.str();
}

std::string Cpu::toHexString(int8_t value) {
    std::ostringstream oss;
    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value & 0xFF);
    return oss.str();
}

void Cpu::_cpu() {

    uint8_t F = (z ? 0x80 : 0x00)  // Bit 7 - Zero Flag
              | (n ? 0x40 : 0x00)  // Bit 6 - Subtract Flag
              | (h ? 0x20 : 0x00)  // Bit 5 - Half Carry Flag
              | (c ? 0x10 : 0x00); // Bit 4 - Carry Flag

    AF = (AF & 0xFF00) | F;

    memcpy(&OAM[0], &Memory[0xFE00], sizeof(OAM));
    memcpy(&VRAM, &Memory[0x8000], sizeof(VRAM));

    SCY = Memory[0xFF42];
    SCX = Memory[0xFF43];
    LY = Memory[0xFF44];
    LYC = Memory[0xFF45];
    WY = Memory[0xFF4A];
    WX = Memory[0xFF4B];
    LCDC = Memory[0xFF40];
    ActiveTileMap = Memory[0xFF40] & 0x40  ? 0 : 1;

     // Palette de fond (Background)
    uint8_t bgp = Memory[0xFF47];
    BG_Palette[0] = (bgp >> 0) & 0x03; // Couleur 0
    BG_Palette[1] = (bgp >> 2) & 0x03; // Couleur 1
    BG_Palette[2] = (bgp >> 4) & 0x03; // Couleur 2
    BG_Palette[3] = (bgp >> 6) & 0x03; // Couleur 3

    // Sprite Palette 0 (0BP0)
    uint8_t obp0 = Memory[0xFF48];
    OBJ_Palette_0[0] = (obp0 >> 0) & 0x03; // Couleur 0
    OBJ_Palette_0[1] = (obp0 >> 2) & 0x03; // Couleur 1
    OBJ_Palette_0[2] = (obp0 >> 4) & 0x03; // Couleur 2
    OBJ_Palette_0[3] = (obp0 >> 6) & 0x03; // Couleur 3

    // Sprite Palette 1 (0BP1)
    uint8_t obp1 = Memory[0xFF49];
    OBJ_Palette_1[0] = (obp1 >> 0) & 0x03; // Couleur 0
    OBJ_Palette_1[1] = (obp1 >> 2) & 0x03; // Couleur 1
    OBJ_Palette_1[2] = (obp1 >> 4) & 0x03; // Couleur 2
    OBJ_Palette_1[3] = (obp1 >> 6) & 0x03; // Couleur 3


}


void Cpu::AddOpcodeEntry(uint16_t address, uint8_t id, const std::string& description, uint16_t opcode) {
    opcodeHistory.push_back(std::make_tuple(address, id, description, opcode));
    if (opcodeHistory.size() > 18) { // Limiter la taille de l'historique
        opcodeHistory.erase(opcodeHistory.begin());
    }
}
