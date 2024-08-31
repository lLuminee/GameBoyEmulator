#pragma once 
#include <iostream>
#include <stdint.h>
#include "../include/cpu.h"
#include "../include/CB_opcode.h"

class Opcode
{
private:
    CB_op Cb_Op;

    void JP_a16(Cpu* cpu, uint16_t opcode);
    void CCF(Cpu* cpu);
    void LD_SP_d16(Cpu* cpu, uint16_t opcode);
    void CALL_a16(Cpu* cpu, uint16_t opcode);
    void LD_HL_d16(Cpu* cpu, uint16_t opcode);
    void LDH_A_a8(Cpu* cpu, uint16_t opcode);
    void CP_d8(Cpu* cpu, uint16_t opcode);
    void JR_NZ_r8(Cpu* cpu, uint16_t opcode);
    void DI(Cpu* cpu); //TODO -------------------------------------------------
    void RET_Z(Cpu* cpu);
    void RET(Cpu* cpu);
    void XOR_A(Cpu* cpu);
    void LDH_a8_A(Cpu* cpu, uint16_t opcode);
    void LD_A_d8(Cpu* cpu, uint16_t opcode);
    void LD_BC_d16(Cpu* cpu, uint16_t opcode);
    void LD_D_A(Cpu* cpu);
    void LD_HL_Plus_A(Cpu* cpu);
    void OR_A_C(Cpu* cpu);
    void LD_A_D(Cpu* cpu);
    void DEC_BC(Cpu* cpu);
    void LD_A_B(Cpu* cpu);
    void LD_C_u8(Cpu* cpu, uint16_t opcode); 
    void DEC_C(Cpu* cpu);
    void JR_i8(Cpu* cpu, uint16_t opcode);
    void LD_A_HL_Plus(Cpu* cpu);
    void LD_DE_d16(Cpu* cpu, uint16_t opcode);
    void JR_d8(Cpu* cpu, uint16_t opcode);
    void LD_DE_A(Cpu* cpu);
    void INC_DE(Cpu* cpu);
    void INC_A(Cpu* cpu);
    void LD_L_u8(Cpu* cpu, uint16_t opcode);
    void LD_B_u8(Cpu* cpu, uint16_t opcode);
    void EI(Cpu* cpu); // TODO -------------------------------------------------
    void HALT(Cpu* cpu); // TODO -------------------------------------------------


public:

    void SearchOpcode(Cpu* cpu, uint16_t opcode, uint8_t id);
    void OpcodeStep(bool isStep, Opcode &op, Cpu &cpu);

};
