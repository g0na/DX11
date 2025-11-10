#include "Maptool_Defines.h"
#include "GameInstance.h"
#include "ImGui_Panel_Hierarchy.h"
#include "GameObject.h"

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
    _uint iCurLevelID = m_pGameInstance->Get_CurLevelID();
    map<const _wstring, CBase*> mapPrototypes = m_pGameInstance->Get_Prototypes(iCurLevelID);

    ImGui::SetNextWindowSize(ImVec2(700, 350), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_strLabel.c_str()))
    {
        // Left
        {
            ImGui::BeginChild("left pane", ImVec2(350, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            for (auto& Pair : mapPrototypes)
            {
                if (dynamic_cast<CGameObject*>(Pair.second) != nullptr)
                {
                    _char szPrototypeName[128] = {};
                    WideCharToMultiByte(CP_ACP, 0, Pair.first.c_str(), -1, szPrototypeName, sizeof(szPrototypeName), nullptr, nullptr);
                    sprintf_s(szPrototypeName, sizeof(szPrototypeName), szPrototypeName);

                    if (ImGui::Selectable(szPrototypeName, false, ImGuiSelectableFlags_SelectOnNav))
                        strcpy_s(m_szSelectedObj, szPrototypeName);
                }
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            //ImGui::Text();
            ImGui::Separator();
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Selected Object"))
                {
                    ImGui::TextWrapped(m_szSelectedObj);
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
