#include "../../include/OpcodeHistory.h"
#include "../../include/cpu.h"
#include <imgui.h>
#include <algorithm> 
#include <cstdint> 



OpcodeHistory::OpcodeHistory(Cpu& cpu) : cpu(cpu) {}

void OpcodeHistory::Render(Cpu& cpu) {
    ImGui::Begin("Opcode Liste",nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse );
    ImGui::SetWindowSize(ImVec2(400, 360), ImGuiCond_FirstUseEver);
    ImGui::SetWindowPos(ImVec2(1200, 0));

    if (ImGui::BeginTable("OpcodeHistoryTable", 4)) { // Spécifiez 4 colonnes
        // Configuration des colonnes
        ImGui::TableSetupColumn("Adresse");
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("jsp"); // Nom de la colonne "jsp" pour l'exemple, à adapter selon vos besoins
        ImGui::TableSetupColumn("Opcode");
        ImGui::TableHeadersRow(); // Ajoutez les en-têtes de colonnes

        // Ajoutez les lignes de la table
        for (const auto& entry : cpu.opcodeHistory) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%04X", std::get<0>(entry)); // Adresse
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%02X", std::get<1>(entry)); // ID
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", std::get<2>(entry).c_str()); // jsp
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%04X", std::get<3>(entry)); // Opcode
        }

        ImGui::EndTable();
    }

    ImGui::End();
}
