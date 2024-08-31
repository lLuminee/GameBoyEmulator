#include "../include/opcode.h"
#include "../include/cpu.h"
#include "../include/CB_opcode.h"

// 0x06

void Opcode::JP_a16(Cpu* cpu, uint16_t adresse) {
    auto LSB = (adresse & 0xFF00) >> 8;
    auto MSB = adresse & 0x00FF;
    uint16_t addr = MSB << 8 | LSB;
    cpu->pc = addr;
    std::cout << "FAIT ! " << cpu->toHexString(addr) << std::endl;
}

void Opcode::CCF(Cpu* cpu) {
    cpu->n = 0x0000;
    cpu->h = 0x0000;
    cpu->c = !cpu->c;
    std::cout << "FAIT !" << std::endl;

}

void Opcode::LD_SP_d16(Cpu* cpu, uint16_t opcode) {
    auto LSB = (opcode & 0xFF00) >> 8;
    auto MSB = (opcode & 0x00FF) << 8;
    cpu->stack.push_back(MSB | LSB);
    std::cout << "FAIT ! " << cpu->toHexString(opcode) << std::endl;

}

    void Opcode::CALL_a16(Cpu* cpu, uint16_t adresse) {
        auto LSB = (adresse & 0xFF00) >> 8;
        auto MSB = adresse & 0x00FF;
        uint16_t addr = MSB << 8 | LSB;
        cpu->LastCalls = cpu->pc + 3;
        cpu->stack.push_back(cpu->pc + 3 /* Call = 3 bytes*/);
        cpu->pc = addr;
        std::cout << "FAIT ! " << cpu->toHexString(addr) << std::endl;
    }

void Opcode::LD_HL_d16(Cpu* cpu, uint16_t opcode) {
    auto LSB = (opcode & 0xFF00) >> 8;
    auto MSB = opcode & 0x00FF;
    uint16_t addr = MSB << 8 | LSB;
    cpu->HL = addr;
    std::cout << "FAIT ! " << cpu->toHexString(opcode) << std::endl;
}

void Opcode::LDH_A_a8(Cpu* cpu, uint16_t opcode) {
    uint8_t a8 = (opcode & 0xFF00) >> 8;
    uint16_t addr = 0xFF00 | a8;
    cpu->AF = (cpu->Memory[addr] << 8) | (cpu->AF & 0x00FF);
    std::cout << "FAIT ! " << cpu->toHexString(a8) << std::endl;
}

void Opcode::CP_d8(Cpu* cpu, uint16_t opcode) {
    uint8_t a8 = (opcode & 0xFF00) >> 8;
    uint8_t A = (cpu->AF & 0xFF00) >> 8;
    uint8_t result = A - a8;
    cpu->z = (result == 0) ? 1 : 0;
    cpu->n = 1;
    cpu->h = ((A & 0x0F) < (a8 & 0x0F)) ? 1 : 0;
    cpu->c = (A < a8) ? 1 : 0;
    std::cout << "FAIT ! " << cpu->toHexString(a8) <<" - " << cpu->toHexString(A) << std::endl;
}

void Opcode::JR_NZ_r8(Cpu* cpu, uint16_t opcode) {
    int8_t r8 = static_cast<int8_t>((opcode & 0xFF00) >> 8);
    if (!cpu->z) {
        cpu->pc += r8 + 2;
    } else {
        cpu->pc += 2;
    }
    std::cout << "FAIT ! " << cpu->toHexString(r8) <<  std::endl;
}

void Opcode::DI(Cpu* cpu) {
    cpu->IME = 0;
    std::cout << "TODO" << std::endl;
}

void Opcode::RET_Z(Cpu* cpu) {
    if (cpu->z == 1) {
        cpu->pc = cpu->stack.back();
        cpu->stack.pop_back();
        std::cout << "FAIT !" << std::endl;
    }
}

void Opcode::RET(Cpu* cpu) {
    cpu->pc = cpu->stack.back();
    cpu->stack.pop_back();
    std::cout << "FAIT !" << std::endl;
}

void Opcode::XOR_A(Cpu* cpu) {

    uint8_t A = (cpu->AF & 0xFF00) >> 8;
    uint8_t result = A ^ A;
    cpu->AF = (cpu->AF & 0x00FF) | (result << 8);

    std::cout << "FAIT !" << std::endl;
    cpu->z = 1;
    cpu->n = 0; 
    cpu->h = 0; 
    cpu->c = 0;     
}

void Opcode::LDH_a8_A(Cpu* cpu, uint16_t opcode) {
    
    uint16_t addr = 0xFF00 | ((opcode & 0xFF00) >> 8);
    if (addr == 0xFF41) {
        auto bit = (cpu->Memory[addr]) & 0x3;
        cpu->Memory[addr] = ((cpu->AF & 0xFF00 ) >> 8) | bit;
    } else {
        cpu->Memory[addr] = (cpu->AF & 0xFF00) >> 8;
 
    }
    std::cout << "FAIT !" << std::endl;

}

void Opcode::LD_A_d8(Cpu* cpu, uint16_t opcode) {
    uint8_t a8 = (opcode & 0xFF00) >> 8;
    cpu->AF = (cpu->AF & 0x00FF) | (a8 << 8);
    std::cout << "FAIT !" << std::endl;
}


void Opcode::LD_BC_d16(Cpu* cpu, uint16_t opcode) {
    auto LSB = (opcode & 0xFF00) >> 8;
    auto MSB = opcode & 0x00FF;
    uint16_t addr = MSB << 8 | LSB;
    cpu->BC = addr;
    std::cout << "FAIT ! " << cpu->toHexString(opcode) << std::endl;
}

void Opcode::LD_D_A(Cpu* cpu) {
    uint16_t A = (cpu->AF & 0xFF00);
    cpu->DE = A | cpu->DE;

    std::cout << "FAIT !" << std::endl;
}

void Opcode::LD_HL_Plus_A(Cpu* cpu) {
    uint8_t A = (cpu->AF & 0xFF00) >> 8;
    cpu->Memory[cpu->HL] = A;
    cpu->HL++;

    std::cout << "FAIT !" << std::endl;
}

void Opcode::OR_A_C(Cpu* cpu) {
    uint8_t A = (cpu->AF & 0xFF00) >> 8;
    uint8_t C = (cpu->BC & 0x00FF);
    cpu->AF = (cpu->AF & 0x00FF) | ((A | C) << 8);

  
    cpu->z = ((A | C) == 0x00) ? 1 : 0;
    cpu->n = 0;
    cpu->h = 0;
    cpu->c = 0;

    std::cout << "FAIT !" << std::endl;
}


void Opcode::LD_A_D(Cpu* cpu) {

    cpu->AF = (cpu->DE & 0xFF00) | (cpu->AF & 0x00FF);

    std::cout << "FAIT !" << std::endl;
    
}

void Opcode::DEC_BC(Cpu* cpu) {
    cpu->BC = cpu->BC - 1;
     std::cout << "FAIT !" << std::endl;

}

void Opcode::LD_A_B(Cpu* cpu) {

    cpu->AF = (cpu->BC & 0xFF00) | (cpu->AF & 0x00FF);
    std::cout << "FAIT !" << std::endl;
}


void Opcode::LD_C_u8(Cpu* cpu, uint16_t opcode) {
    uint8_t u8 = (opcode & 0xFF00) >> 8;
    cpu->BC = (cpu->BC & 0xFF00) | u8;
    std::cout << "FAIT !" << std::endl;

}

void Opcode::DEC_C(Cpu* cpu) {
    uint8_t C = cpu->BC & 0x00FF;

    C = (C - 1) & 0xFF;  

    cpu->BC = (cpu->BC & 0xFF00) | C; 

    cpu->z = (C == 0) ? 1 : 0;
    cpu->n = 1;
    cpu->h = ((C & 0x0F) == 0x0F) ? 1 : 0;

    std::cout << "FAIT !" << std::endl;
}

void Opcode::JR_i8(Cpu* cpu, uint16_t opcode) {
    int8_t i8 = static_cast<int8_t>((opcode & 0xFF00) >> 8);
    cpu->pc = cpu->pc + i8;
    std::cout << "FAIT !" << std::endl;
}

void Opcode::LD_A_HL_Plus(Cpu* cpu) {
    uint8_t value = cpu->Memory[cpu->HL];
    cpu->HL = cpu->HL + 1;
    cpu->AF = (value << 8) | (cpu->AF & 0x00FF);
    std::cout << "FAIT !" << std::endl;
}

void Opcode::LD_DE_d16(Cpu* cpu, uint16_t opcode) {
    auto LSB = (opcode & 0xFF00) >> 8;
    auto MSB = opcode & 0x00FF;
    uint16_t addr = MSB << 8 | LSB;
    cpu->DE = addr;
    std::cout << "FAIT ! " << cpu->toHexString(opcode) << std::endl;
}

void Opcode::JR_d8(Cpu* cpu, uint16_t opcode) {
    int8_t i8 = static_cast<int8_t>((opcode & 0xFF00) >> 8);
    cpu->pc = cpu->pc + i8;
    std::cout << "FAIT !" << std::endl;
}

void Opcode::LD_DE_A(Cpu* cpu) {
    cpu->Memory[cpu->DE] = (cpu->AF & 0xFF00) >> 8;
    std::cout << "FAIT !" << std::endl;
}

void Opcode::INC_DE(Cpu* cpu) {
    cpu->DE = cpu->DE + 1;
    std::cout << "FAIT !" << std::endl;
}

void Opcode::INC_A(Cpu* cpu) {
    uint8_t A = (cpu->AF & 0xFF00) >> 8;
    A = (A + 1) & 0xFF; // Incrémenter A en gérant l'overflow sur 8 bits
    cpu->AF = (cpu->AF & 0x00FF) | (A << 8);
    uint8_t H = (A & 0x0F) == 0x0F; // Half-carry si les 4 bits de poids faible étaient 0x0F
    uint8_t Z = (A == 0); // Zero flag

    cpu->z = Z;
    cpu->h = H;
    cpu->n = 0;

    std::cout << "FAIT !" << std::endl;
}

void Opcode::LD_L_u8(Cpu* cpu, uint16_t opcode) {
    uint8_t u8 = (opcode & 0xFF00) >> 8;
    cpu->HL = (cpu->HL & 0xFF00) | u8;
    std::cout << "FAIT !" << std::endl;
}

void Opcode::LD_B_u8(Cpu* cpu, uint16_t opcode) {
    uint8_t u8 = (opcode & 0xFF00) >> 8;
    cpu->BC = (cpu->BC & 0x00FF) | (u8 << 8);
    std::cout << "FAIT !" << std::endl;
}

void Opcode::EI(Cpu* cpu) {
    cpu->IME = 1;
    // Todo
}

void Opcode::HALT(Cpu* cpu) {
    
}

void Opcode::SearchOpcode(Cpu* cpu, uint16_t Opcode, uint8_t id) {
    cpu->OpCycle = 20;
    cpu->opcode = Opcode;
    cpu->last_pc = cpu->pc;
    
    switch (cpu->id)
    {
    case 0x00:
        std::cout << "FAIT !" << std::endl;
        cpu->OpcodeName = "NOP";
        cpu->pc += 1;
        break;
    case 0x01:
        LD_BC_d16(cpu, Opcode);
        cpu->OpcodeName = "LD BC,d16";
        cpu->pc += 3;
        break;
    case 0x02:
        cpu->OpcodeName = "LD (BC),A";
        break;
    case 0x03:
        cpu->OpcodeName = "INC BC";
        break;
    case 0x04:
        cpu->OpcodeName = "INC B";
        break;
    case 0x05:
        cpu->OpcodeName = "DEC B";
        break;
    case 0x06:
        LD_B_u8(cpu, Opcode);
        cpu->OpcodeName = "LD B,d8";
        cpu->pc += 2;
        break;
    case 0x07:
        cpu->OpcodeName = "RLCA";
        break;
    case 0x08:
        cpu->OpcodeName = "LD (a16),SP";
        break;
    case 0x09:
        cpu->OpcodeName = "ADD HL,BC";
        break;
    case 0x0A:
        cpu->OpcodeName = "LD A,(BC)";
        break;
    case 0x0B:
        DEC_BC(cpu); 
        cpu->OpcodeName = "DEC BC";
        cpu->pc += 1;
        break;
    case 0x0C:
        cpu->OpcodeName = "INC C";
        break;
    case 0x0D:
        DEC_C(cpu);
        cpu->OpcodeName = "DEC C";
        cpu->pc += 1;
        break;
    case 0x0E:
        LD_C_u8(cpu, Opcode);
        cpu->OpcodeName = "LD C,d8";
        cpu->pc += 2;
        break;
    case 0x0F:
        cpu->OpcodeName = "RRCA";
        break;
    case 0x10:
        cpu->OpcodeName = "STOP 0";
        break;
    case 0x11:
        LD_DE_d16(cpu, Opcode);
        cpu->OpcodeName = "LD DE,d16";
        cpu->pc += 3;
        break;
    case 0x12:
        LD_DE_A(cpu);
        cpu->OpcodeName = "LD (DE),A";
        cpu->pc += 1;
        break;
    case 0x13:
        INC_DE(cpu);
        cpu->OpcodeName = "INC DE";
        cpu->pc += 1;
        break;
    case 0x14:
        cpu->OpcodeName = "INC D";
        break;
    case 0x15:
        cpu->OpcodeName = "DEC D";
        break;
    case 0x16:
        cpu->OpcodeName = "LD D,d8";
        break;
    case 0x17:
        cpu->OpcodeName = "RLA";
        break;
    case 0x18:
        JR_d8(cpu, Opcode);
        cpu->OpcodeName = "JR d8";
        cpu->pc += 2;
        break;
    case 0x19:
        cpu->OpcodeName = "ADD HL,DE";
        break;
    case 0x1A:
        cpu->OpcodeName = "LD A,(DE)";
        break;
    case 0x1B:
        cpu->OpcodeName = "DEC DE";
        break;
    case 0x1C:
        cpu->OpcodeName = "INC E";
        break;
    case 0x1D:
        cpu->OpcodeName = "DEC E";
        break;
    case 0x1E:
        cpu->OpcodeName = "LD E,d8";
        break;
    case 0x1F:
        cpu->OpcodeName = "RRA";
        break;
    case 0x20:
        JR_NZ_r8(cpu, Opcode);
        cpu->OpcodeName = "JR NZ,r8";
        break;
    case 0x21:
        LD_HL_d16(cpu, Opcode);
        cpu->OpcodeName = "LD HL,d16";
        cpu->pc += 3;
        break;
    case 0x22:
        LD_HL_Plus_A(cpu);
        cpu->OpcodeName = "LD_HL+_A";
        cpu->pc += 1;
        break;
    case 0x23:
        cpu->OpcodeName = "INC HL";
        break;
    case 0x24:
        cpu->OpcodeName = "INC H";
        break;
    case 0x25:
        cpu->OpcodeName = "DEC H";
        break;
    case 0x26:
        cpu->OpcodeName = "LD H,d8";
        break;
    case 0x27:
        cpu->OpcodeName = "DAA";
        break;
    case 0x28:
        cpu->OpcodeName = "JR Z,r8";
        break;
    case 0x29:
        cpu->OpcodeName = "ADD HL,HL";
        break;
    case 0x2A:
        LD_A_HL_Plus(cpu);
        cpu->OpcodeName = "LD HL,(a16)";
        cpu->pc += 1;
        break;
    case 0x2B:
        cpu->OpcodeName = "DEC HL";
        break;
    case 0x2C:
        cpu->OpcodeName = "INC L";
        break;
    case 0x2D:
        cpu->OpcodeName = "DEC L";
        break;
    case 0x2E:
        LD_L_u8(cpu, Opcode);
        cpu->OpcodeName = "LD L,d8";
        cpu->pc += 2;
        break;
    case 0x2F:
        cpu->OpcodeName = "CPL";
        break;
    case 0x30:
        cpu->OpcodeName = "JR NC,r8";
        break;
    case 0x31:
        LD_SP_d16(cpu, Opcode);
        cpu->OpcodeName = "LD SP,d16";
        cpu->pc += 3;
        break;
    case 0x32:
        cpu->OpcodeName = "LD (a16),A";
        break;
    case 0x33:
        cpu->OpcodeName = "INC SP";
        break;
    case 0x34:
        cpu->OpcodeName = "INC (HL)";
        break;
    case 0x35:
        cpu->OpcodeName = "DEC (HL)";
        break;
    case 0x36:
        cpu->OpcodeName = "LD (HL),d8";
        break;
    case 0x37:
        cpu->OpcodeName = "SCF";
        break;
    case 0x38:
        cpu->OpcodeName = "JR C,r8";
        break;
    case 0x39:
        cpu->OpcodeName = "ADD HL,SP";
        break;
    case 0x3A:
        cpu->OpcodeName = "LD A,(a16)";
        break;
    case 0x3B:
        cpu->OpcodeName = "DEC SP";
        break;
    case 0x3C:
        INC_A(cpu);
        cpu->OpcodeName = "INC A";
        cpu->pc += 1;
        break;
    case 0x3D:
        cpu->OpcodeName = "DEC A";
        break;
    case 0x3E:
        LD_A_d8(cpu, Opcode);
        cpu->OpcodeName = "LD A,d8";
        cpu->pc += 2;
        break;
    case 0x3F:
        CCF(cpu);
        cpu->OpcodeName = "CCF";
        cpu->pc += 1;
        break;
    case 0x40:
        cpu->OpcodeName = "LD B,B";
        break;
    case 0x41:
        cpu->OpcodeName = "LD B,C";
        break;
    case 0x42:
        cpu->OpcodeName = "LD B,D";
        break;
    case 0x43:
        cpu->OpcodeName = "LD B,E";
        break;
    case 0x44:
        cpu->OpcodeName = "LD B,H";
        break;
    case 0x45:
        cpu->OpcodeName = "LD B,L";
        break;
    case 0x46:
        cpu->OpcodeName = "LD B,(HL)";
        break;
    case 0x47:
        cpu->OpcodeName = "LD B,A";
        break;
    case 0x48:
        cpu->OpcodeName = "LD C,B";
        break;
    case 0x49:
        cpu->OpcodeName = "LD C,C";
        break;
    case 0x4A:
        cpu->OpcodeName = "LD C,D";
        break;
    case 0x4B:
        cpu->OpcodeName = "LD C,E";
        break;
    case 0x4C:
        cpu->OpcodeName = "LD C,H";
        break;
    case 0x4D:
        cpu->OpcodeName = "LD C,L";
        break;
    case 0x4E:
        cpu->OpcodeName = "LD C,(HL)";
        break;
    case 0x4F:
        cpu->OpcodeName = "LD C,A";
        break;
    case 0x50:
        cpu->OpcodeName = "LD D,B";
        break;
    case 0x51:
        cpu->OpcodeName = "LD D,C";
        break;
    case 0x52:
        cpu->OpcodeName = "LD D,D";
        break;
    case 0x53:
        cpu->OpcodeName = "LD D,E";
        break;
    case 0x54:
        cpu->OpcodeName = "LD D,H";
        break;
    case 0x55:
        cpu->OpcodeName = "LD D,L";
        break;
    case 0x56:
        cpu->OpcodeName = "LD D,(HL)";
        break;
    case 0x57:
        LD_D_A(cpu);
        cpu->OpcodeName = "LD D,A";
        cpu->pc += 1;
        break;
    case 0x58:
        cpu->OpcodeName = "LD E,B";
        break;
    case 0x59:
        cpu->OpcodeName = "LD E,C";
        break;
    case 0x5A:
        cpu->OpcodeName = "LD E,D";
        break;
    case 0x5B:
        cpu->OpcodeName = "LD E,E";
        break;
    case 0x5C:
        cpu->OpcodeName = "LD E,H";
        break;
    case 0x5D:
        cpu->OpcodeName = "LD E,L";
        break;
    case 0x5E:
        cpu->OpcodeName = "LD E,(HL)";
        break;
    case 0x5F:
        cpu->OpcodeName = "LD E,A";
        break;
    case 0x60:
        cpu->OpcodeName = "LD H,B";
        break;
    case 0x61:
        cpu->OpcodeName = "LD H,C";
        break;
    case 0x62:
        cpu->OpcodeName = "LD H,D";
        break;
    case 0x63:
        cpu->OpcodeName = "LD H,E";
        break;
    case 0x64:
        cpu->OpcodeName = "LD H,H";
        break;
    case 0x65:
        cpu->OpcodeName = "LD H,L";
        break;
    case 0x66:
        cpu->OpcodeName = "LD H,(HL)";
        break;
    case 0x67:
        cpu->OpcodeName = "LD H,A";
        break;
    case 0x68:
        cpu->OpcodeName = "LD L,B";
        break;
    case 0x69:
        cpu->OpcodeName = "LD L,C";
        break;
    case 0x6A:
        cpu->OpcodeName = "LD L,D";
        break;
    case 0x6B:
        cpu->OpcodeName = "LD L,E";
        break;
    case 0x6C:
        cpu->OpcodeName = "LD L,H";
        break;
    case 0x6D:
        cpu->OpcodeName = "LD L,L";
        break;
    case 0x6E:
        cpu->OpcodeName = "LD L,(HL)";
        break;
    case 0x6F:
        cpu->OpcodeName = "LD L,A";
        break;
    case 0x70:
        cpu->OpcodeName = "LD (HL),B";
        break;
    case 0x71:
        cpu->OpcodeName = "LD (HL),C";
        break;
    case 0x72:
        cpu->OpcodeName = "LD (HL),D";
        break;
    case 0x73:
        cpu->OpcodeName = "LD (HL),E";
        break;
    case 0x74:
        cpu->OpcodeName = "LD (HL),H";
        break;
    case 0x75:
        cpu->OpcodeName = "LD (HL),L";
        break;
    case 0x76:
        HALT(cpu);
        cpu->OpcodeName = "HALT";
        cpu->pc += 1;
        break;
    case 0x77:
        cpu->OpcodeName = "LD (HL),A";
        break;
    case 0x78:
        LD_A_B(cpu);
        cpu->OpcodeName = "LD A,B";
        cpu->pc += 1;
        break;
    case 0x79:
        cpu->OpcodeName = "LD A,C";
        break;
    case 0x7A:
        LD_A_D(cpu);
        cpu->OpcodeName = "LD A,D";
        cpu->pc += 1;
        break;
    case 0x7B:
        cpu->OpcodeName = "LD A,E";
        break;
    case 0x7C:
        cpu->OpcodeName = "LD A,H";
        break;
    case 0x7D:
        cpu->OpcodeName = "LD A,L";
        break;
    case 0x7E:
        cpu->OpcodeName = "LD A,(HL)";
        break;
    case 0x7F:
        cpu->OpcodeName = "LD A,A";
        break;
    case 0x80:
        cpu->OpcodeName = "ADD A,B";
        break;
    case 0x81:
        cpu->OpcodeName = "ADD A,C";
        break;
    case 0x82:
        cpu->OpcodeName = "ADD A,D";
        break;
    case 0x83:
        cpu->OpcodeName = "ADD A,E";
        break;
    case 0x84:
        cpu->OpcodeName = "ADD A,H";
        break;
    case 0x85:
        cpu->OpcodeName = "ADD A,L";
        break;
    case 0x86:
        cpu->OpcodeName = "ADD A,(HL)";
        break;
    case 0x87:
        cpu->OpcodeName = "ADD A,A";
        break;
    case 0x88:
        cpu->OpcodeName = "ADC A,B";
        break;
    case 0x89:
        cpu->OpcodeName = "ADC A,C";
        break;
    case 0x8A:
        cpu->OpcodeName = "ADC A,D";
        break;
    case 0x8B:
        cpu->OpcodeName = "ADC A,E";
        break;
    case 0x8C:
        cpu->OpcodeName = "ADC A,H";
        break;
    case 0x8D:
        cpu->OpcodeName = "ADC A,L";
        break;
    case 0x8E:
        cpu->OpcodeName = "ADC A,(HL)";
        break;
    case 0x8F:
        cpu->OpcodeName = "ADC A,A";
        break;
    case 0x90:
        cpu->OpcodeName = "SUB B";
        break;
    case 0x91:
        cpu->OpcodeName = "SUB C";
        break;
    case 0x92:
        cpu->OpcodeName = "SUB D";
        break;
    case 0x93:
        cpu->OpcodeName = "SUB E";
        break;
    case 0x94:
        cpu->OpcodeName = "SUB H";
        break;
    case 0x95:
        cpu->OpcodeName = "SUB L";
        break;
    case 0x96:
        cpu->OpcodeName = "SUB (HL)";
        break;
    case 0x97:
        cpu->OpcodeName = "SUB A";
        break;
    case 0x98:
        cpu->OpcodeName = "SBC A,B";
        break;
    case 0x99:
        cpu->OpcodeName = "SBC A,C";
        break;
    case 0x9A:
        cpu->OpcodeName = "SBC A,D";
        break;
    case 0x9B:
        cpu->OpcodeName = "SBC A,E";
        break;
    case 0x9C:
        cpu->OpcodeName = "SBC A,H";
        break;
    case 0x9D:
        cpu->OpcodeName = "SBC A,L";
        break;
    case 0x9E:
        cpu->OpcodeName = "SBC A,(HL)";
        break;
    case 0x9F:
        cpu->OpcodeName = "SBC A,A";
        break;
    case 0xA0:
        cpu->OpcodeName = "AND B";
        break;
    case 0xA1:
        cpu->OpcodeName = "AND C";
        break;
    case 0xA2:
        cpu->OpcodeName = "AND D";
        break;
    case 0xA3:
        cpu->OpcodeName = "AND E";
        break;
    case 0xA4:
        cpu->OpcodeName = "AND H";
        break;
    case 0xA5:
        cpu->OpcodeName = "AND L";
        break;
    case 0xA6:
        cpu->OpcodeName = "AND (HL)";
        break;
    case 0xA7:
        cpu->OpcodeName = "AND A";
        break;
    case 0xA8:
        cpu->OpcodeName = "XOR B";
        break;
    case 0xA9:
        cpu->OpcodeName = "XOR C";
        break;
    case 0xAA:
        cpu->OpcodeName = "XOR D";
        break;
    case 0xAB:
        cpu->OpcodeName = "XOR E";
        break;
    case 0xAC:
        cpu->OpcodeName = "XOR H";
        break;
    case 0xAD:
        cpu->OpcodeName = "XOR L";
        break;
    case 0xAE:
        cpu->OpcodeName = "XOR (HL)";
        break;
    case 0xAF:
        XOR_A(cpu);
        cpu->OpcodeName = "XOR A";
        cpu->pc += 1;
        break;
    case 0xB0:
        cpu->OpcodeName = "OR B";
        break;
    case 0xB1:
        OR_A_C(cpu);
        cpu->OpcodeName = "OR C";
        cpu->pc += 1;
        break;
    case 0xB2:
        cpu->OpcodeName = "OR D";
        break;
    case 0xB3:
        cpu->OpcodeName = "OR E";
        break;
    case 0xB4:
        cpu->OpcodeName = "OR H";
        break;
    case 0xB5:
        cpu->OpcodeName = "OR L";
        break;
    case 0xB6:
        cpu->OpcodeName = "OR (HL)";
        break;
    case 0xB7:
        cpu->OpcodeName = "OR A";
        break;
    case 0xB8:
        cpu->OpcodeName = "CP B";
        break;
    case 0xB9:
        cpu->OpcodeName = "CP C";
        break;
    case 0xBA:
        cpu->OpcodeName = "CP D";
        break;
    case 0xBB:
        cpu->OpcodeName = "CP E";
        break;
    case 0xBC:
        cpu->OpcodeName = "CP H";
        break;
    case 0xBD:
        cpu->OpcodeName = "CP L";
        break;
    case 0xBE:
        cpu->OpcodeName = "CP (HL)";
        break;
    case 0xBF:
        cpu->OpcodeName = "CP A";
        break;
    case 0xC0:
        cpu->OpcodeName = "RET NZ";
        break;
    case 0xC1:
        cpu->OpcodeName = "POP BC";
        break;
    case 0xC2:
        cpu->OpcodeName = "JP NZ,a16";
        break;
    case 0xC3:
        cpu->OpcodeName = "JP a16";
        JP_a16(cpu, Opcode);
        break;
    case 0xC4:
        cpu->OpcodeName = "CALL NZ,a16";
        break;
    case 0xC5:
        cpu->OpcodeName = "PUSH BC";
        break;
    case 0xC6:
        cpu->OpcodeName = "ADD A,d8";
        break;
    case 0xC7:
        cpu->OpcodeName = "RST 00H";
        break;
    case 0xC8:
        RET_Z(cpu);
        cpu->OpcodeName = "RET Z";
        cpu->pc += 1;
        break;
    case 0xC9:
        RET(cpu);
        cpu->OpcodeName = "RET";
        break;
    case 0xCA:
        cpu->OpcodeName = "JP Z,a16";
        break;
    case 0xCB:
        Cb_Op.SearchCBOpcode(cpu, Opcode, cpu->Memory[cpu->pc + 1]);
        cpu->OpcodeName = "PREFIX CB";
        break;
    case 0xCC:
        cpu->OpcodeName = "CALL Z,a16";
        break;
    case 0xCD:
        CALL_a16(cpu, Opcode);
        cpu->OpcodeName = "CALL a16";
        break;
    case 0xCE:
        cpu->OpcodeName = "ADC A,d8";
        break;
    case 0xCF:
        cpu->OpcodeName = "RST 08H";
        break;
    case 0xD0:
        cpu->OpcodeName = "RET NC";
        break;
    case 0xD1:
        cpu->OpcodeName = "POP DE";
        break;
    case 0xD2:
        cpu->OpcodeName = "JP NC,a16";
        break;
    case 0xD3:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xD4:
        cpu->OpcodeName = "CALL NC,a16";
        break;
    case 0xD5:
        cpu->OpcodeName = "PUSH DE";
        break;
    case 0xD6:
        cpu->OpcodeName = "SUB d8";
        break;
    case 0xD7:
        cpu->OpcodeName = "RST 10H";
        break;
    case 0xD8:
        cpu->OpcodeName = "RET C";
        break;
    case 0xD9:
        cpu->OpcodeName = "RETI";
        break;
    case 0xDA:
        cpu->OpcodeName = "JP C,a16";
        break;
    case 0xDB:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xDC:
        cpu->OpcodeName = "CALL C,a16";
        break;
    case 0xDD:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xDE:
        cpu->OpcodeName = "SBC A,d8";
        break;
    case 0xDF:
        cpu->OpcodeName = "RST 18H";
        break;
    case 0xE0:
        LDH_a8_A(cpu, Opcode);
        cpu->OpcodeName = "LDH (a8),A";
        cpu->pc += 2;
        break;
    case 0xE1:
        cpu->OpcodeName = "POP HL";
        break;
    case 0xE2:
        cpu->OpcodeName = "LD (C),A";
        break;
    case 0xE3:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xE4:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xE5:
        cpu->OpcodeName = "PUSH HL";
        break;
    case 0xE6:
        cpu->OpcodeName = "AND d8";
        break;
    case 0xE7:
        cpu->OpcodeName = "RST 20H";
        break;
    case 0xE8:
        cpu->OpcodeName = "ADD SP,r8";
        break;
    case 0xE9:
        cpu->OpcodeName = "JP (HL)";
        break;
    case 0xEA:
        cpu->OpcodeName = "LD (a16),A";
        break;
    case 0xEB:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xEC:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xED:
        cpu->OpcodeName = "UNKNOWN";
        break;
    case 0xEE:
        cpu->OpcodeName = "XOR d8";
        break;
    case 0xEF:
        cpu->OpcodeName = "RST 28H";
        break;
    case 0xF0:
        LDH_A_a8(cpu, Opcode);
        cpu->OpcodeName = "LDH A,(a8)";
        cpu->pc += 2;
        break;
    case 0xF1:
        cpu->OpcodeName = "POP AF";
        break;
    case 0xF2:
        cpu->OpcodeName = "LDH A,(C)";
        break;
    case 0xF3:
        DI(cpu);
        cpu->pc += 1;
        cpu->OpcodeName = "DI";
        break;
    case 0xF5:
        cpu->OpcodeName = "PUSH AF";
        break;
    case 0xF6:
        cpu->OpcodeName = "OR d8";
        break;
    case 0xF7:
        cpu->OpcodeName = "RST 30H";
        break;
    case 0xF8:
        cpu->OpcodeName = "LD HL,SP+r8";
        break;
    case 0xF9:
        cpu->OpcodeName = "LD SP,HL";
        break;
    case 0xFA:
        cpu->OpcodeName = "LD A,(a16)";
        break;
    case 0xFB:
        EI(cpu);
        cpu->OpcodeName = "EI";
        cpu->pc += 1;
        break;
    case 0xFE:
        CP_d8(cpu, Opcode);
        cpu->OpcodeName = "CP d8";
        cpu->pc += 2;
        break;
    case 0xFF:
        cpu->OpcodeName = "RST 38H";
        break;

    default:
        break;
    }
}



void Opcode::OpcodeStep(bool isStep, Opcode &op, Cpu &cpu)  {

    if (cpu.PasCall) {
        if (cpu.pc != 0x0157) {
            cpu.isStep = false;
        } else {
            cpu.PasCall = false;
            cpu.isStep = true;

        }
    }

    if (cpu.Step && isStep) {
        cpu.state = 2;
        cpu.Step = false;
        
        uint16_t Opcode = cpu.Memory[cpu.pc + 1] << 8 | cpu.Memory[cpu.pc + 2];
        cpu.id = cpu.Memory[cpu.pc]; 
        op.SearchOpcode(&cpu, Opcode, cpu.id);
        cpu.NextOpcode = cpu.Memory[cpu.pc + 1] << 8 | cpu.Memory[cpu.pc + 2];
        cpu.NextId = cpu.Memory[cpu.pc];
        cpu.AddOpcodeEntry(cpu.last_pc, cpu.id, std::string(cpu.OpcodeName), cpu.opcode);   

        }

    if (!isStep) {
        cpu.state = 0;

        uint16_t Opcode = cpu.Memory[cpu.pc + 1] << 8 | cpu.Memory[cpu.pc + 2];
        cpu.id = cpu.Memory[cpu.pc]; 
        op.SearchOpcode(&cpu, Opcode, cpu.id);
        cpu.NextOpcode = cpu.Memory[cpu.pc + 1] << 8 | cpu.Memory[cpu.pc + 2];
        cpu.NextId = cpu.Memory[cpu.pc];
        cpu.AddOpcodeEntry(cpu.last_pc, cpu.id, std::string(cpu.OpcodeName), cpu.opcode);   

  
    }
}