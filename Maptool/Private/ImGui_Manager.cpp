#include "Maptool_Defines.h"
#include "ImGui_Panel_Inspector.h"
#include "ImGui_Panel_Prototype.h"
#include "ImGui_Panel_Scene.h"
#include "ImGui_Manager.h"
#include "ImGui_Panel.h"

#include "GameInstance.h"
#include "Calculator.h"
#include "Layer.h"
#include "GameObject.h"

_bool g_bIsCreatable = false;
CGameObject* g_pSelectedPrototype = { nullptr };
_char g_szSelectedPrototypeName[128] = "";

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Initialize_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	// ImGui Context 세팅
	IMGUI_CHECKVERSION();
	ImGuiContext *pContext = ImGui::CreateContext();
	if (!pContext)
		return E_FAIL;

	m_pIO = &ImGui::GetIO(); (void)m_pIO;
	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	m_pIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	m_pIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	m_pIO->IniFilename = NULL;

	// ImGui Style 세팅
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	// Win32용 초기화
	if (!::ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	// dx11용 초기화
	if (!::ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext))
		return E_FAIL;

	if (FAILED(Ready_Panels()))
		return E_FAIL;

	return S_OK;
}

void CImGui_Manager::Render()
{
	// win32, dx11, imguicontext 짝꿍
	::ImGui_ImplDX11_NewFrame();
	::ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Create a dockspace in main viewport, where central node is transparent.
	ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	// 첫 실행 시 도킹 레이아웃 설정
	static bool first_time = true;
	if (first_time)
	{
		first_time = false;

		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

		// 좌측 20%: Prototype 패널
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);

		// 우측 30%: Inspector 패널
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);

		// 중앙 하단 30%: Scene 패널
		ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, nullptr, &dockspace_id);

		// 각 패널 도킹
		ImGui::DockBuilderDockWindow("PROTOTYPE", dock_id_left);
		ImGui::DockBuilderDockWindow("SCENE", dock_id_bottom);
		ImGui::DockBuilderDockWindow("INSPECTOR", dock_id_right);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	////////////////////////
	// 마우스 픽킹 관련 함수
	if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB) &&
		!g_bIsCreatable)
		Picking_GameObject();

	Render_Panels();
	////////////////////////

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (m_pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

HRESULT CImGui_Manager::Ready_Panels()
{
	m_pCalculator = CCalculator::Create(m_pDevice, m_pDeviceContext);
	if (m_pCalculator == nullptr)
		return E_FAIL;

	m_pPanels[ENUM_TO_UINT(PANEL::PROTOTYPE)] = CImGui_Panel_Prototype::Create();
	m_pPanels[ENUM_TO_UINT(PANEL::SCENE)] = CImGui_Panel_Scene::Create(m_pDevice, m_pDeviceContext);
	m_pPanels[ENUM_TO_UINT(PANEL::INSPECTOR)] = CImGui_Panel_Inspector::Create();

	static_cast<CImGui_Panel_Scene*>(m_pPanels[ENUM_TO_UINT(PANEL::SCENE)])->Set_Inspector(
		static_cast<CImGui_Panel_Inspector*>(m_pPanels[ENUM_TO_UINT(PANEL::INSPECTOR)]));

	return S_OK;
}

void CImGui_Manager::Render_Panels()
{
	for (CImGui_Panel* pPanel : m_pPanels)
	{
		if(pPanel)
			pPanel->Render();
	}
}

void CImGui_Manager::Picking_GameObject()
{
	map<const _wstring, class CLayer*>* pLayers = { nullptr };
	list<CGameObject*> listObjects;

	pLayers = m_pGameInstance->Get_Layers();

	_float fMinDist = { 9999.f };
	_vector vFinalPos = {};
	CGameObject* pFinalObject = { nullptr };

	// 모든 레이어를 검사하는건 별로 같음. 현재 씬의 레이어만 검사할 수 있도록 바꾸자
	for (auto& Pair : pLayers[ENUM_TO_UINT(LEVELID::GAMEPLAY)])
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

	if (pFinalObject != nullptr)
	{
		char buf[512];
		sprintf_s(buf, "x: %f, y: %f, z %f, Obj: %ls\n", XMVectorGetX(vFinalPos), XMVectorGetY(vFinalPos), XMVectorGetZ(vFinalPos), pFinalObject->Get_Name());
		OutputDebugStringA(buf);

		if (pFinalObject->Get_Layer() == TEXT("Layer_Map"))
			return;

		static_cast<CImGui_Panel_Inspector*>(m_pPanels[ENUM_TO_UINT(PANEL::INSPECTOR)])->Set_SelectedObject(pFinalObject);
		static_cast<CImGui_Panel_Scene*>(m_pPanels[ENUM_TO_UINT(PANEL::SCENE)])->Set_SelectedObject(pFinalObject);
	}
}

void CImGui_Manager::Free()
{
	__super::Free();

	for (auto& pPanel : m_pPanels)
		Safe_Release(pPanel);

	::ImGui_ImplDX11_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pCalculator);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}