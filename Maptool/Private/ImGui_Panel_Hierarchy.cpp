#include "Maptool_Defines.h"
#include "GameInstance.h"
#include "ImGui_Panel_Hierarchy.h"
#include "GameObject.h"
#include "Calculator.h"
#include "Layer.h"

CImGui_Panel_Hierarchy::CImGui_Panel_Hierarchy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CImGui_Panel("HIERARCHY")
    , m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CImGui_Panel_Hierarchy::~CImGui_Panel_Hierarchy()
{
}

HRESULT CImGui_Panel_Hierarchy::Initialize()
{
    m_pCalculator = CCalculator::Create(m_pDevice, m_pContext);
    if (m_pCalculator == nullptr)
        return E_FAIL;

    return S_OK;
}

void CImGui_Panel_Hierarchy::Render()
{
    _uint iCurLevelID = m_pGameInstance->Get_CurLevelID();
    map<const _wstring, CBase*> mapPrototypes = m_pGameInstance->Get_Prototypes(iCurLevelID);

    ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_strLabel.c_str()))
    {
        // Left
        {
            ImGui::BeginChild("left pane", ImVec2(250, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            for (auto& Pair : mapPrototypes)
            {
                if (dynamic_cast<CGameObject*>(Pair.second) != nullptr)
                {
                    _char szPrototypeName[128] = {};
                    WideCharToMultiByte(CP_ACP, 0, Pair.first.c_str(), -1, szPrototypeName, sizeof(szPrototypeName), nullptr, nullptr);
                    sprintf_s(szPrototypeName, sizeof(szPrototypeName), szPrototypeName);

                    if (ImGui::Selectable(szPrototypeName, false, ImGuiSelectableFlags_SelectOnNav))
                    {
                        g_bIsCreatable = true;
                        m_pSelectedObject = static_cast<CGameObject*>(Pair.second);
                        strcpy_s(m_szSelectedObj, szPrototypeName);
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Text("Create Mode");
            ImGui::Separator();
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Selected Object"))
                {
                    if (g_bIsCreatable)
                        ImGui::TextWrapped(m_szSelectedObj);
                    else
                        ImGui::TextWrapped("No Selected Object!");

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();

            if (ImGui::Button("Exit Create Mode")) 
            {
                strcpy_s(m_szSelectedObj, "");
                g_bIsCreatable = false;
                m_pSelectedObject = nullptr;
            }
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
            ImGui::EndGroup();
        }
    }

	// 픽킹한 위치에 모델 띄우기
	if (g_bIsCreatable && 
        !ImGui::GetIO().WantCaptureMouse &&
        m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB))
	{
		map<const _wstring, class CLayer*>* pLayers = { nullptr };
		list<CGameObject*> listObjects = { nullptr };

		pLayers = m_pGameInstance->Get_Layers();

		_float fMinDist = { 9999.f };
		_vector vFinalPos = {};
		CGameObject* pFinalObject = { nullptr };

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
					_float	fDist = { 0.f };

					_vector	vPickPos = m_pCalculator->Picking_OnMesh(g_hWnd, pMeshCom, pTransformCom, fDist);

                    if (XMVectorGetW(vPickPos) > 0.f &&
                        fDist <= fMinDist)
                    {
                        // 최소 거리 갱신
                        fMinDist = fDist;
                        // 픽킹 좌표와 오브젝트를 최종 CGameObject 변수와 _vector 변수에 대입
                        pFinalObject = pObject;
                        vFinalPos = vPickPos;
                    }
				}
			}
		}

		CGameObject* pGameObject = m_pGameInstance->Add_GameObject_To_Layer(iCurLevelID, CharToWstring(m_szSelectedObj), iCurLevelID, m_pSelectedObject->Get_Layer());
		if (pGameObject == nullptr)
		{
			MSG_BOX("Failed to place GameObject");
			return;
		}
        else
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(g_strTransformTag);
			if (pTransform != nullptr)
				pTransform->Set_State(STATE::POSITION, vFinalPos);
		}
	}

	ImGui::End();
}

_wstring CImGui_Panel_Hierarchy::CharToWstring(const _char* pString)
{
    _uint iStringSize = MultiByteToWideChar(CP_ACP, 0, pString, -1, NULL, 0);

    _wstring strResult(iStringSize - 1, 0);
    MultiByteToWideChar(CP_ACP, 0, pString, -1, &strResult[0], iStringSize);

    return strResult;
}

CImGui_Panel_Hierarchy* CImGui_Panel_Hierarchy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImGui_Panel_Hierarchy* pInstance = new CImGui_Panel_Hierarchy(pDevice, pContext);

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

    Safe_Release(m_pCalculator);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
