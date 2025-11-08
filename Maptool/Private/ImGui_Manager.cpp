#include "Maptool_Defines.h"
#include "ImGui_Panel_Inspector.h"
#include "ImGui_Panel_Hierarchy.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "Calculator.h"
#include "Layer.h"
#include "GameObject.h"

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
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	////////////////////////
	// 초반에는 켜놓고 기능 찾아보는 것 추천
	ImGui::ShowDemoWindow();

	// 마우스 픽킹 관련 함수
	if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
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

	m_pPanels[ENUM_TO_UINT(PanelType::INSPECTOR)] = CImGui_Panel_Inspector::Create();
	m_pPanels[ENUM_TO_UINT(PanelType::HIERARCHY)] = CImGui_Panel_Hierarchy::Create();
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
	list<CGameObject*> listObjects = { nullptr };

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
