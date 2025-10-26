#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

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
	EngineDesc		m_tEngineDesc {};
	m_tEngineDesc.hInstance = g_hInst;
	m_tEngineDesc.hWnd = g_hWnd;
	m_tEngineDesc.eWinMode = WINMODE::WIN;
	m_tEngineDesc.iWinSizeX = g_iWinSizeX;
	m_tEngineDesc.iWinSizeY = g_iWinSizeY;
	m_tEngineDesc.iLevelNum = ENUM_TO_UINT(LEVELID::END);
	
	if (FAILED(m_pGameInstance->Initialize_Engine(m_tEngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_For_Static()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVELID::LOGO)))
		return E_FAIL; 

	return S_OK;
}

void CMainApp::Update(const _float& fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4	vColor = { 0.f, 0.f, 0.f, 0.f };

	if (FAILED(m_pGameInstance->Draw_Begin(vColor)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_End()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVELID eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(ENUM_TO_UINT(LEVELID::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Static()
{
	// Prototype_Component_VIBuffer_Rect 추가
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_Component_Shader_VtxPosTex 추가
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
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

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	// MainApp의 멤버를 정리한다.
	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
