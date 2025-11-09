#include "Maptool_Defines.h"
#include "GameInstance.h"
#include "ImGui_Panel_Hierarchy.h"

CImGui_Panel_Hierarchy::CImGui_Panel_Hierarchy()
    : CImGui_Panel("HIERARCHY")
{
}

CImGui_Panel_Hierarchy::~CImGui_Panel_Hierarchy()
{
}

HRESULT CImGui_Panel_Hierarchy::Initialize()
{
    return S_OK;
}

void CImGui_Panel_Hierarchy::Render()
{
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_strLabel.c_str()))
    {
        // Left
        static int selected = 0;
        {
            ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            for (int i = 0; i < 100; i++)
            {
                char label[128];
                sprintf_s(label, sizeof(label), "MyObject %d", i);
                if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SelectOnNav))
                    selected = i;
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Text("MyObject: %d", selected);
            ImGui::Separator();
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Description"))
                {
                    ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Details"))
                {
                    ImGui::Text("ID: 0123456789");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
            if (ImGui::Button("Revert")) {}
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}

CImGui_Panel_Hierarchy* CImGui_Panel_Hierarchy::Create()
{
    CImGui_Panel_Hierarchy* pInstance = new CImGui_Panel_Hierarchy();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CImGui_Panel_Hierarchy::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CImGui_Panel_Hierarchy::Free()
{
    __super::Free();
}
