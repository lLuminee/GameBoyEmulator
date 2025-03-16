#pragma once 
#include <iostream>
#include <stdint.h>
#include "../include/cpu.h"

class CB_op
{
private:

    void BIT_7_HL(Cpu* cpu, uint16_t opcode);
    void RES_7_HL(Cpu* cpu, uint16_t opcode);
    void BIT_6_HL(Cpu* cpu);
    void SET_6_HL(Cpu* cpu);
    void SET_1_HL(Cpu* cpu);
    void SET_4_HL(Cpu* cpu);
    void SET_7_HL(Cpu* cpu);
    void RES_0_HL(Cpu* cpu);
    void SET_0_HL(Cpu* cpu);
    void SET_5_HL(Cpu* cpu);
    void RES_4_HL(Cpu* cpu);
    void SET_2_A(Cpu* cpu);
    void SET_2_HL(Cpu* cpu);
    void RES_2_HL(Cpu* cpu);
    void RES_0_E(Cpu* cpu);
    void RES_1_HL(Cpu* cpu);
    void RES_6_HL(Cpu* cpu);
    void SET_3_HL(Cpu* cpu);
    void RES_5_HL(Cpu* cpu);
    void RES_3_HL(Cpu* cpu);



public:

    void SearchCBOpcode(Cpu* cpu, uint16_t opcode, uint8_t id);

};
