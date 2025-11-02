#include "Client_Defines.h"
#include "ImGui_Panel_Inspector.h"
#include "ImGui_Panel_Hierarchy.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

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

void CImGui_Manager::Free()
{
	__super::Free();

	for (CImGui_Panel* pPanel : m_pPanels)
	{
		Safe_Release(pPanel);
	}

	::ImGui_ImplDX11_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
