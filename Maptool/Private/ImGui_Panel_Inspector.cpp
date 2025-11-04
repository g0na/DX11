#include "Client_Defines.h"
#include "GameInstance.h"
#include "Transform.h"
#include "ImGui_Panel_Inspector.h"

CImGui_Panel_Inspector::CImGui_Panel_Inspector()
    : CImGui_Panel("INSPECTOR")
{
}

CImGui_Panel_Inspector::~CImGui_Panel_Inspector()
{
}

HRESULT CImGui_Panel_Inspector::Intiailzie()
{
    return S_OK;
}

void CImGui_Panel_Inspector::Render()
{
    // Begin과 End는 짝을 이룬다.
    ImGui::Begin(m_strLabel.c_str());

    ImGui::Text("Inspector");
    ImGui::Text("hi");

    // Todo - 절대 지울것
    static _uint iCount = { 0 };

    if (ImGui::Button("Create"))
    {
        CGameInstance::GetInstance()->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
            ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Monster"));
    }

    // Todo - 나중에 피킹으로 처리해서 오브젝트나 Grid를 얻어와 버튼과 상호작용 시킬 수 있도록 해야함
    const LIGHT_DESC* pDesc = CGameInstance::GetInstance()->Get_LightDesc(0);
    // 예시를 위한 흑마술
    LIGHT_DESC* pDarkDesc = const_cast<LIGHT_DESC*>(pDesc);

    if (ImGui::Button("Plus Diffuse X"))
    {

    }
    {
        // 다음에 배치되는 gui 같은 라인에 둬!
        ImGui::SameLine();
    }
    if (ImGui::Button("Minus Diffuse Y"))
    {
        pDarkDesc->vDiffuse.x -= 0.01f;
    }
    {
        // 수직 간격
        ImGui::Spacing();
        ImGui::Spacing();
    }
    if (ImGui::Button("Clear Diffuse"))
    {
        pDarkDesc->vDiffuse.x = 1.f;
        pDarkDesc->vDiffuse.y = 1.f;
        pDarkDesc->vDiffuse.z = 1.f;
    }
    {
        // 줄 바꿈
        ImGui::NewLine();
        // 수평 구분선
        ImGui::Separator();
        ImGui::NewLine();
    }
    if (ImGui::Button("Red Diffuse"))
    {
        pDarkDesc->vDiffuse.x = 1.f;
        pDarkDesc->vDiffuse.y = 0.f;
        pDarkDesc->vDiffuse.z = 0.f;
    }

    if (ImGui::Button("Blue Diffuse"))
    {
        pDarkDesc->vDiffuse.x = 0.f;
        pDarkDesc->vDiffuse.y = 1.f;
        pDarkDesc->vDiffuse.z = 0.f;
    }

    if (ImGui::Button("Green Diffuse"))
    {
        pDarkDesc->vDiffuse.x = 0.f;
        pDarkDesc->vDiffuse.y = 0.f;
        pDarkDesc->vDiffuse.z = 1.f;
    }

    // 표
    if (ImGui::BeginTable("Props", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        auto Row = [&](const char* label, auto widget) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted(label);
            ImGui::TableSetColumnIndex(1); widget();
        };

        static char name[64] = "Player";
        Row("Name", [&] { ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputText("##name", name, sizeof(name)); });

        static float speed = 5.0f;
        Row("Speed", [&] { ImGui::SetNextItemWidth(-FLT_MIN); ImGui::DragFloat("##speed", &speed, 0.1f, 0.0f, 100.0f); });

        ImGui::EndTable();
    }


    ImGui::End();
}

CImGui_Panel_Inspector* CImGui_Panel_Inspector::Create()
{
    CImGui_Panel_Inspector* pInstance = new CImGui_Panel_Inspector();
    if (FAILED(pInstance->Intiailzie()))
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
