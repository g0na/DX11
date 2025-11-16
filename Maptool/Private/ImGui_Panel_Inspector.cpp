#include "Maptool_Defines.h"
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
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(m_strLabel.c_str()))
    {
        // TODO: 선택된 오브젝트가 있을 때만 표시
        bool bHasSelection = false; // 선택된 오브젝트 여부

        if (!bHasSelection)
        {
            ImGui::TextDisabled("No object selected");
        }
        else
        {
            // 오브젝트 이름
            ImGui::Text("Object Name");
            ImGui::SameLine();
            static char objName[128] = "GameObject";
            ImGui::InputText("##ObjName", objName, IM_ARRAYSIZE(objName));

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Transform 컴포넌트
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                // Position
                ImGui::Text("Position");
                static float position[3] = { 0.0f, 0.0f, 0.0f };
                ImGui::DragFloat3("##Position", position, 0.1f);

                ImGui::Spacing();

                // Rotation
                ImGui::Text("Rotation");
                static float rotation[3] = { 0.0f, 0.0f, 0.0f };
                ImGui::DragFloat3("##Rotation", rotation, 1.0f, -180.0f, 180.0f);

                ImGui::Spacing();

                // Scale
                ImGui::Text("Scale");
                static float scale[3] = { 1.0f, 1.0f, 1.0f };
                ImGui::DragFloat3("##Scale", scale, 0.01f, 0.001f, 100.0f);

                ImGui::Unindent();
            }

            ImGui::Spacing();

            // Model 컴포넌트
            if (ImGui::CollapsingHeader("Model"))
            {
                ImGui::Indent();
                ImGui::Text("Model Path: ");
                ImGui::SameLine();
                ImGui::TextDisabled("../Resources/Models/Example.fbx");
                ImGui::Unindent();
            }

            ImGui::Spacing();

            // Shader 컴포넌트
            if (ImGui::CollapsingHeader("Shader"))
            {
                ImGui::Indent();
                ImGui::Text("Shader: ");
                ImGui::SameLine();
                ImGui::TextDisabled("Shader_VtxNorTex");
                ImGui::Unindent();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // 액션 버튼
            if (ImGui::Button("Apply Changes", ImVec2(-1, 30)))
            {
                // TODO: 변경사항 적용
            }

            if (ImGui::Button("Delete Object", ImVec2(-1, 30)))
            {
                // TODO: 오브젝트 삭제
            }
        }
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
