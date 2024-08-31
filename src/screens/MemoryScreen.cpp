#include "../../include/MemoryScreen.h"
#include <imgui_memory_editor.h>
#include <imgui.h>

MemoryScreen::MemoryScreen(Cpu& cpu) : cpu(cpu) {
}

void MemoryScreen::Render() {
    static MemoryEditor mem_edit;
    mem_edit.Cols = 24;

    ImGui::Begin("Memory",nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse );

    ImGui::SetWindowSize(ImVec2(800, 360));
    ImGui::SetWindowPos(ImVec2(800, 360));
    mem_edit.DrawContents(cpu.Memory, sizeof(cpu.Memory));

    ImGui::End();
}
