#include "MainApp.h"

USING(Client)

Client::CMainApp::CMainApp() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

Client::CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	// EngineDesc 초기화
	m_tEngineDesc.hWnd = g_hWnd;
	m_tEngineDesc.eWinMode = WINMODE::WIN;
	m_tEngineDesc.iWinSizeX = g_iWinSizeX;
	m_tEngineDesc.iWinSizeY = g_iWinSizeY;
	
	if (FAILED(m_pGameInstance->Initialize_Engine(m_tEngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(const _float& fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4	vColor = { 0.f, 0.f, 1.f, 1.f };
	if (FAILED(m_pGameInstance->Draw_Begin(vColor)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_End()))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;
	
	if (FAILED(pMainApp->Initialize()))
	{
		MSG_BOX("Failed to create CMainApp!");
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void Client::CMainApp::Free()
{
	__super::Free();

	// MainApp의 멤버를 정리한다.
	Safe_Release(m_pGameInstance);
}
