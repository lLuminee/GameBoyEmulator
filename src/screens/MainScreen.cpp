#include "../../include/MainScreen.h"

#include <imgui.h>

MainScreen::MainScreen(Cpu& cpu)
    : cpu(cpu),
      debugScreen(cpu),
      memoryScreen(cpu),
      cartridgeScreen(cpu),
      opcodeHistory(cpu) {}

void MainScreen::Render() {
    ImGui::Begin("Debug",nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse );
    ImGui::BeginTabBar("TEST");

    if (ImGui::BeginTabItem("Debug")) {
        debugScreen.Render();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Cartridge")) {
        cartridgeScreen.Render();
        ImGui::EndTabItem();
    }

    memoryScreen.Render();

    opcodeHistory.Render(cpu);
    

    ImGui::EndTabBar();

    ImGui::End();
}
