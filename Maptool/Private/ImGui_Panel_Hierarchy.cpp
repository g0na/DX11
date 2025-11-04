#include "Client_Defines.h"
#include "GameInstance.h"
#include "ImGui_Panel_Hierarchy.h"

CImGui_Panel_Hierarchy::CImGui_Panel_Hierarchy()
    : CImGui_Panel("HIERARCHY")
{
}

CImGui_Panel_Hierarchy::~CImGui_Panel_Hierarchy()
{
}

HRESULT CImGui_Panel_Hierarchy::Intiailzie()
{
    return S_OK;
}

void CImGui_Panel_Hierarchy::Render()
{
    // Begin과 End는 짝을 이룬다.
    ImGui::Begin(m_strLabel.c_str());

    ImGui::Text("Hierarchy");
    ImGui::Text("bye");

    // Todo - 나중에 피킹으로 처리해서 오브젝트나 Grid를 얻어와 버튼과 상호작용 시킬 수 있도록 해야함


    ImGui::End();
}

CImGui_Panel_Hierarchy* CImGui_Panel_Hierarchy::Create()
{
    CImGui_Panel_Hierarchy* pInstance = new CImGui_Panel_Hierarchy();
    if (FAILED(pInstance->Intiailzie()))
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
