#include "Maptool_Defines.h"
#include "ImGui_Panel_Inspector.h"
#include "GameObject.h"
#include "Transform.h"

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
        bool bHasSelection = (m_pInspectedObject != nullptr);       // 선택된 오브젝트 여부

        if (!bHasSelection)
        {
            ImGui::TextDisabled("No object selected");
        }
        else
        {
            // 오브젝트 이름
            ImGui::Text("Object Name");
            ImGui::SameLine();

            _uint iNameLength = {};
            iNameLength = WideCharToMultiByte(CP_ACP, 0, m_pInspectedObject->Get_Name(), -1, NULL, 0, NULL, NULL);

            static _char objName[128] = { };
            WideCharToMultiByte(CP_ACP, 0, m_pInspectedObject->Get_Name(), -1, objName, iNameLength, NULL, NULL);

            ImGui::InputText("##ObjName", objName, IM_ARRAYSIZE(objName));

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Transform 컴포넌트
            CTransform* pTransform = static_cast<CTransform*>(m_pInspectedObject->Get_Component(g_strTransformTag));

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                // Position
                ImGui::Text("Position");
                _float4 position = {};
                _vector vPosition = pTransform->Get_State(STATE::POSITION);
                XMStoreFloat4(&position, vPosition);

                ImGui::DragFloat3("##Position", (float*)&position, 0.1f);
                pTransform->Set_State(STATE::POSITION, position);      // 위치값 갱신

                ImGui::Spacing();

                // Rotation
                ImGui::Text("Rotation");
                m_vRotationAngle = pTransform->Get_RotationAngle();
                ImGui::DragFloat3("##Rotation", (float*)&m_vRotationAngle, 1.0f, -180.0f, 180.0f);
                pTransform->Rotation(XMConvertToRadians(m_vRotationAngle.x), XMConvertToRadians(m_vRotationAngle.y), XMConvertToRadians(m_vRotationAngle.z));
                pTransform->Set_RotationAngle(m_vRotationAngle);

                ImGui::Spacing();

                // Scale
                ImGui::Text("Scale");
                _float3 scale = {};
                scale = pTransform->Get_Scaled();
                ImGui::DragFloat3("##Scale", (float*)&scale, 0.01f, 0.001f, 100.0f);
                pTransform->Set_Scale(scale.x, scale.y, scale.z);

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
                ImGui::TextDisabled("Shader_VtxMesh");
                ImGui::Unindent();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // 액션 버튼
            if (ImGui::Button("Save", ImVec2(-1, 30)))
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
