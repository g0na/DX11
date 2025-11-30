#include "Maptool_Defines.h"
#include "GameInstance.h"
#include "ImGui_Panel_Prototype.h"
#include "GameObject.h"
#include "Layer.h"

CImGui_Panel_Prototype::CImGui_Panel_Prototype()
    : CImGui_Panel("PROTOTYPE")
{
}

CImGui_Panel_Prototype::~CImGui_Panel_Prototype()
{
}

HRESULT CImGui_Panel_Prototype::Initialize()
{
    return S_OK;
}

void CImGui_Panel_Prototype::Render()
{
    _uint iCurLevelID = m_pGameInstance->Get_CurLevelID();
    map<const _wstring, CBase*> mapPrototypes = m_pGameInstance->Get_Prototypes(iCurLevelID);

    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(m_strLabel.c_str()))
    {
        ImGui::Text("Prototype Objects");
        ImGui::Separator();

        // 검색 필터
        static char search[128] = "";
        ImGui::InputText("Search", search, IM_ARRAYSIZE(search));
        ImGui::Spacing();

        // 프로토타입 목록
        ImGui::BeginChild("PrototypeList", ImVec2(0, -40), ImGuiChildFlags_Borders);

        for (auto& Pair : mapPrototypes)
        {
            if (dynamic_cast<CGameObject*>(Pair.second) != nullptr)
            {
                if (dynamic_cast<CGameObject*>(Pair.second)->Get_Layer() == L"Layer_Monster")
                {
                    _char szPrototypeName[128] = {};
                    WideCharToMultiByte(CP_ACP, 0, Pair.first.c_str(), -1, szPrototypeName, sizeof(szPrototypeName), nullptr, nullptr);

                    // 검색 필터 적용
                    if (strlen(search) > 0 && strstr(szPrototypeName, search) == nullptr)
                        continue;

                    // 클릭 시 선택
                    if (ImGui::Selectable(szPrototypeName))
                    {
                        g_bIsCreatable = true;
                        g_pSelectedPrototype = static_cast<CGameObject*>(Pair.second);
                        strcpy_s(g_szSelectedPrototypeName, szPrototypeName);
                    }

                    // 드래그 앤 드롭도 유지
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                    {
                        ImGui::SetDragDropPayload("PROTOTYPE_DRAG", szPrototypeName, strlen(szPrototypeName) + 1);
                        ImGui::Text("Drag: %s", szPrototypeName);
                        ImGui::EndDragDropSource();
                    }
                }
            }
        }
        ImGui::EndChild();

        // 하단 정보
        //ImGui::Separator();
        //ImGui::Text("Selected Objects : %s", m_szSelectedObj);
    }
    ImGui::End();
}

CImGui_Panel_Prototype* CImGui_Panel_Prototype::Create()
{
    CImGui_Panel_Prototype* pInstance = new CImGui_Panel_Prototype();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CImGui_Panel_Prototype::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CImGui_Panel_Prototype::Free()
{
    __super::Free();

}
