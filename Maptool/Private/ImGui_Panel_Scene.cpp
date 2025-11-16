#include "Maptool_Defines.h"
#include "ImGui_Panel_Scene.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Calculator.h"
#include "Layer.h"

CImGui_Panel_Scene::CImGui_Panel_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CImGui_Panel("SCENE")
    , m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CImGui_Panel_Scene::~CImGui_Panel_Scene()
{
}

HRESULT CImGui_Panel_Scene::Initialize()
{
    m_pCalculator = CCalculator::Create(m_pDevice, m_pContext);
    if (m_pCalculator == nullptr)
        return E_FAIL;

    return S_OK;
}

void CImGui_Panel_Scene::Render()
{
    _uint iCurLevelID = m_pGameInstance->Get_CurLevelID();
    map<const _wstring, class CLayer*>* pLayers = m_pGameInstance->Get_Layers();

    ImGui::SetNextWindowSize(ImVec2(800, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(m_strLabel.c_str()))
    {
        ImGui::Text("Scene Objects");
        ImGui::SameLine(ImGui::GetWindowWidth() - 150);

        // 생성 모드 토글
        if (g_bIsCreatable)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
            if (ImGui::Button("Exit Create Mode", ImVec2(140, 0)))
            {
                g_bIsCreatable = false;
            }
            ImGui::PopStyleColor();
        }
        else
        {
            if (ImGui::Button("Enter Create Mode", ImVec2(140, 0)))
            {
                g_bIsCreatable = true;
            }
        }

        ImGui::Separator();

        // 레이어 탭
        if (ImGui::BeginTabBar("LayerTabs", ImGuiTabBarFlags_None))
        {
            for (auto& LayerPair : pLayers[iCurLevelID])
            {
                _char szLayerName[128] = {};
                WideCharToMultiByte(CP_ACP, 0, LayerPair.first.c_str(), -1, szLayerName, sizeof(szLayerName), nullptr, nullptr);

                if (ImGui::BeginTabItem(szLayerName))
                {
                    // 테이블 헤더
                    if (ImGui::BeginTable("ObjectTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                        ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Rotation", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Scale", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        // 오브젝트 목록
                        list<CGameObject*> listObjects = LayerPair.second->Get_Objects();
                        int id = 0;
                        for (auto pObject : listObjects)
                        {
                            ImGui::TableNextRow();

                            // 선택 여부 확인
                            _bool bIsSelected = m_pSelectedObject == pObject;
                            if (bIsSelected)
                            {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                    ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.9f, 0.5f)));
                            }

                            ImGui::PushID(id++);

                            // 이름
                            ImGui::TableSetColumnIndex(0);
                            _char szObjName[128] = {};
                            WideCharToMultiByte(CP_ACP, 0, pObject->Get_Name(), -1, szObjName, sizeof(szObjName), nullptr, nullptr);        

                            if (ImGui::Selectable(szObjName, bIsSelected, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                // 클릭 시 선택 (Inspector에서 편집할 오브젝트)
                                // TODO: 선택된 오브젝트를 Inspector에 넘겨주자.
                                // 1. 
                                g_bIsCreatable = false;
                                m_pSelectedObject = pObject;
                            }

                            if (ImGui::BeginPopupContextItem())
                            {
                                if (ImGui::MenuItem("Delete"))
                                {
                                    // TODO: 오브젝트 삭제
                                }

                                ImGui::EndPopup();
                            }

                            // Position
                            ImGui::TableSetColumnIndex(1);
                            CTransform* pTransform = pObject->Get_Component<CTransform>(g_strTransformTag);
                            if (pTransform)
                            {
                                _float4 vPos;
                                XMStoreFloat4(&vPos, pTransform->Get_State(STATE::POSITION));
                                ImGui::Text("%.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);
                            }

                            // Rotation
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("0.0, 0.0, 0.0"); // TODO: 회전 값 구현

                            // Scale
                            ImGui::TableSetColumnIndex(3);
                            if (pTransform)
                            {
                                _float4 vScale;
                                XMStoreFloat4(&vScale, pTransform->Get_State(STATE::RIGHT));
                                _float fScaleX = XMVectorGetX(XMVector3Length(pTransform->Get_State(STATE::RIGHT)));
                                _float fScaleY = XMVectorGetX(XMVector3Length(pTransform->Get_State(STATE::UP)));
                                _float fScaleZ = XMVectorGetX(XMVector3Length(pTransform->Get_State(STATE::LOOK)));
                                ImGui::Text("%.2f, %.2f, %.2f", fScaleX, fScaleY, fScaleZ);
                            }
                            ImGui::PopID();
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }

    ImGui::End();

    // 오브젝트 배치 기능 (ImGui 창 밖에서 클릭 시)
    if (g_bIsCreatable &&
        !ImGui::GetIO().WantCaptureMouse &&
        m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB))
    {
        list<CGameObject*> listObjects = { nullptr };

        _float fMinDist = { 9999.f };
        _vector vFinalPos = {};
        CGameObject* pFinalObject = { nullptr };

        // 모든 레이어 순회하며 피킹
        for (auto& Pair : pLayers[iCurLevelID])
        {
            listObjects = Pair.second->Get_Objects();

            for (auto pObject : listObjects)
            {
                CTransform* pTransformCom = pObject->Get_Component<CTransform>(g_strTransformTag);
                if (pTransformCom == nullptr)
                    continue;

                CModel* pModelCom = pObject->Get_Component<CModel>(TEXT("Com_Model"));
                if (pModelCom == nullptr)
                    continue;

                _uint iNumMeshes = pModelCom->Get_NumMeshes();
                for (_uint i = 0; i < iNumMeshes; i++)
                {
                    CMesh* pMeshCom = pModelCom->Get_Mesh(i);
                    _float fDist = { 0.f };

                    _vector vPickPos = m_pCalculator->Picking_OnMesh(g_hWnd, pMeshCom, pTransformCom, fDist);

                    if (XMVectorGetW(vPickPos) > 0.f &&
                        fDist <= fMinDist)
                    {
                        fMinDist = fDist;
                        pFinalObject = pObject;
                        vFinalPos = vPickPos;
                    }
                }
            }
        }

        // 오브젝트 배치 부분
        if (pFinalObject != nullptr && g_pSelectedPrototype != nullptr)
        {
            _wstring strPrototypeName = CharToWstring(g_szSelectedPrototypeName);
            
            // 프로토타입 이름에서 Prototype_GameObject_ 까지만 자름.
            _wstring strObjectName = strPrototypeName.substr(21);
            CGameObject::GAMEOBJECT_DESC GameObjectDesc{};
            lstrcpy(GameObjectDesc.szName, strObjectName.c_str());

            CGameObject* pGameObject = m_pGameInstance->Add_GameObject_To_Layer(iCurLevelID, strPrototypeName, iCurLevelID, g_pSelectedPrototype->Get_Layer(), &GameObjectDesc);

            if (pGameObject != nullptr)
            {
                CTransform* pTransform = pGameObject->Get_Component<CTransform>(g_strTransformTag);
                if (pTransform != nullptr)
                    pTransform->Set_State(STATE::POSITION, vFinalPos);
            }
        }
    }
}

_wstring CImGui_Panel_Scene::CharToWstring(const _char* pString)
{
    _uint iStringSize = MultiByteToWideChar(CP_ACP, 0, pString, -1, NULL, 0);

    _wstring strResult(iStringSize - 1, 0);
    MultiByteToWideChar(CP_ACP, 0, pString, -1, &strResult[0], iStringSize);

    return strResult;
}

CImGui_Panel_Scene* CImGui_Panel_Scene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImGui_Panel_Scene* pInstance = new CImGui_Panel_Scene(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CImGui_Panel_Scene::Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CImGui_Panel_Scene::Free()
{
    __super::Free();

    Safe_Release(m_pCalculator);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
