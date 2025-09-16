#include "MainApp.h"

USING(Client)

Client::CMainApp::CMainApp()
{
}

Client::CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	CGameInstance::GetInstance()->Initialize_Device(g_hWnd, WINMODE::WIN, g_iWinSizeX, g_iWinSizeY, &m_pDevice, &m_pContext);
	return S_OK;
}

void CMainApp::Update(const _float& fTimeDelta)
{
}

HRESULT CMainApp::Render()
{
	if (FAILED(CGameInstance::GetInstance()->Draw()))
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

}
