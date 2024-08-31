#include "../../include/DebugScreen.h"
#include <imgui.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include <nfd.h>

DebugScreen::DebugScreen(Cpu& cpu) : cpu(cpu) {}

void DebugScreen::Render() {

    ImGui::SetWindowSize(ImVec2(400, 360), ImGuiCond_FirstUseEver);
    ImGui::SetWindowPos(ImVec2(800, 0));

    ImGui::Text("CPU State: ");
    ImGui::SameLine();
    if(cpu.state == 0) ImGui::TextColored(ImVec4(0.00f, 0.42f, 1.00f, 1.00f), "RUNNING");
    else if(cpu.state == 1) ImGui::TextColored(ImVec4(1.00f, 0.48f, 0.00f, 1.00f), "PAUSED");
    else if(cpu.state == 2) ImGui::TextColored(ImVec4(1.00f, 0.00f, 0.97f, 1.00f), "STEP");

    if (ImGui::Button("Pause")) {
        if (cpu.isStep) {
            cpu.isStep = false;
        }
        else {
            cpu.isStep = true;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Step")) {
        if (cpu.Step) {
            cpu.Step = false;
        }
        else {
            cpu.Step = true;
        }
    }  

    ImGui::SameLine();
    if (ImGui::Button("Jump")) {
        cpu.PasCall = true;
    } 
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        cpu.IsReset = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Load Rom")) {


        // Déclarez un pointeur pour stocker le chemin du fichier sélectionné
        nfdchar_t *outPath = nullptr;
        // Ouvrez la boîte de dialogue pour sélectionner un fichier avec les extensions "ch8" et "c8"
        nfdresult_t result = NFD_OpenDialog("gb", nullptr, &outPath);
        // Gérer le résultat de la boîte de dialogue
        switch (result) {
            case NFD_OKAY:
                std::cout << "Fichier sélectionné: " << outPath << std::endl;
                cpu.Init(outPath);
                free(outPath);  // Libérez la mémoire allouée pour le chemin du fichier
                break;
            case NFD_CANCEL:
                std::cout << "L'utilisateur a annulé la sélection." << std::endl;
                break;
            case NFD_ERROR:
                std::cerr << "Erreur: " << NFD_GetError() << std::endl;
                break;
        }        

    }

    ImGui::Separator();
    ImGui::Text("Next Instruction:");
    ImGui::Text("ID : 0x%02X", cpu.NextId);
    ImGui::Text("0x%04X", cpu.NextOpcode);

    ImGui::Separator();
    ImGui::Text("Flags:");
    ImGui::Columns(4, "flags0", false);
    int z = static_cast<int>(cpu.z);
    if (ImGui::InputInt("z", &z)) {
        cpu.z = static_cast<bool>(z);
    }
    ImGui::NextColumn();

    int n = static_cast<int>(cpu.n);
    if (ImGui::InputInt("n", &n)) {
        cpu.n = static_cast<bool>(n);
    }
    ImGui::NextColumn();

    int h = static_cast<int>(cpu.h);
    if (ImGui::InputInt("h", &h)) {
        cpu.h = static_cast<bool>(h);
    }
    ImGui::NextColumn();

    int c_Carry = static_cast<int>(cpu.c);
    if (ImGui::InputInt("c", &c_Carry)) {
        cpu.c = static_cast<bool>(c_Carry);
    }
    ImGui::Columns(1);    
    ImGui::Dummy(ImVec2(0.0f, 5.0f));  
    ImGui::Text("Registers");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));  

    ImGui::Columns(2, "Registers0", false);
    
    if (cpu.stack.size() > 0){
        cpu.sp = cpu.stack.back();
    } else {cpu.sp = 0x0000;}
    char sp_[16];
    sprintf(sp_, "0x%04X", cpu.sp);
    if (ImGui::InputText("SP", sp_, sizeof(sp_), ImGuiInputTextFlags_CharsHexadecimal)) {
        sscanf(sp_, "%x", &cpu.sp);
    }
    ImGui::NextColumn();
    char pc_[16];
    sprintf(pc_, "0x%04X", cpu.pc);
    if (ImGui::InputText("PC", pc_, sizeof(pc_), ImGuiInputTextFlags_CharsHexadecimal)) {
        sscanf(pc_, "%x", &cpu.pc);
    }
    ImGui::NextColumn();
    ImGui::Columns(1); 
    ImGui::Columns(2, "Registers1", false);
    char A_[16];
    sprintf(A_, "0x%02X", (cpu.AF & 0xFF00) >> 8);
    if (ImGui::InputText("A", A_, sizeof(A_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(A_, "%x", &value);
        cpu.AF = (cpu.AF & 0x00FF) | (value << 8);
    }
    ImGui::NextColumn(); 
    char F_[16];
    sprintf(F_, "0x%02X", (cpu.AF & 0x00FF));
    if (ImGui::InputText("F", F_, sizeof(F_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(F_, "%x", &value);
        cpu.AF = (cpu.AF & 0xFF00) | value;
    }
    ImGui::NextColumn();
    ImGui::Columns(1);   
    ImGui::Columns(2, "Registers2", false);
    char B_[16];
    sprintf(B_, "0x%02X", (cpu.BC & 0xFF00) >> 8);
    if (ImGui::InputText("B", B_, sizeof(B_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(B_, "%x", &value);
        cpu.BC = (cpu.BC & 0x00FF) | (value << 8);
    }
    ImGui::NextColumn(); 
    char C_[16];
    sprintf(C_, "0x%02X", (cpu.BC & 0x00FF));
    if (ImGui::InputText("C", C_, sizeof(C_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(C_, "%x", &value);
        cpu.BC = (cpu.BC & 0xFF00) | value;
    }
    ImGui::NextColumn();
    ImGui::Columns(1);   
    ImGui::Columns(2, "Registers3", false);
    char D_[16];
    sprintf(D_, "0x%02X", (cpu.DE & 0xFF00) >> 8);
    if (ImGui::InputText("D", D_, sizeof(D_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(D_, "%x", &value);
        cpu.DE = (cpu.DE & 0x00FF) | (value << 8);
    }
    ImGui::NextColumn(); 
    char E_[16];
    sprintf(E_, "0x%02X", (cpu.DE & 0x00FF));
    if (ImGui::InputText("E", E_, sizeof(E_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(E_, "%x", &value);
        cpu.DE = (cpu.DE & 0xFF00) | value;
    }
    ImGui::NextColumn();
    ImGui::Columns(1);   
    ImGui::Columns(2, "Registers4", false);
    char H_[16];
    sprintf(H_, "0x%02X", (cpu.HL & 0xFF00) >> 8);
    if (ImGui::InputText("H", H_, sizeof(H_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(H_, "%x", &value);
        cpu.HL = (cpu.HL & 0x00FF) | (value << 8);
    }
    ImGui::NextColumn(); 
    char L_[16];
    sprintf(L_, "0x%02X", (cpu.HL & 0x00FF));
    if (ImGui::InputText("L", L_, sizeof(L_), ImGuiInputTextFlags_CharsHexadecimal)) {
        int value;
        sscanf(L_, "%x", &value);
        cpu.HL = (cpu.HL & 0xFF00) | value;
    }
    ImGui::NextColumn();
    ImGui::Columns(1);

}
