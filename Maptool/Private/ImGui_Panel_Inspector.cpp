#include "Maptool_Defines.h"
#include "GameInstance.h"
#include "Calculator.h"
#include "Layer.h"
#include "GameObject.h"
#include "ImGui_Panel_Inspector.h"

CImGui_Panel_Inspector::CImGui_Panel_Inspector()
    : CImGui_Panel("INSPECTOR")
{
}

CImGui_Panel_Inspector::~CImGui_Panel_Inspector()
{
}

HRESULT CImGui_Panel_Inspector::Initialize()
{
    

    return S_OK;
}

void CImGui_Panel_Inspector::Render()
{
    _uint iCurLevelID = m_pGameInstance->Get_CurLevelID();
    map<const _wstring, CBase*> mapPrototypes = m_pGameInstance->Get_Prototypes(iCurLevelID);

    _uint iNumPrototypes = (_uint)mapPrototypes.size();

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);

    // Begin°ú End´Â Â¦À» ÀÌ·é´Ù.
    if (ImGui::Begin(m_strLabel.c_str()))
    {
        // Left
        static int selected = 0;
        {
            ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            for (auto& Pair : mapPrototypes)
            {                           
                //char label[128];
                //sprintf_s(label, sizeof(label), "MyObject %d", i);
                //if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SelectOnNav))
                //    selected = i;
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

    }

    ImGui::End();
}

CImGui_Panel_Inspector* CImGui_Panel_Inspector::Create()
{
    CImGui_Panel_Inspector* pInstance = new CImGui_Panel_Inspector();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CImGui_Panel_Inspector::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CImGui_Panel_Inspector::Free()
{
    __super::Free();
}
