#include "GameInstance.h"
#include "TimerManager.h"
#include "Graphic_Device.h"

USING(Engine)

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
};

#pragma region ENGINE
HRESULT	CGameInstance::Initialize_Engine(EngineDesc& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext)
{
	// 그래픽 디바이스 초기화
	m_pGraphicDevice = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppDeviceContext);
	if (m_pGraphicDevice == nullptr)
		return E_FAIL;

	// 인풋 디바이스 초기화
	// 사운드 디바이스 초기화
	// 타이머 매니저 초기화
	m_pTimerManager = CTimerManager::Create();
	if (m_pTimerManager == nullptr)
		return E_FAIL;

	// 프로토타입 매니저 초기화
	// 오브젝트 , 매니저 초기화
	return S_OK;
}

void CGameInstance::Update_Engine(const _float& fTimeDelta)
{

}

HRESULT CGameInstance::Draw_Begin(const _float4& vColor)
{
	if (FAILED(m_pGraphicDevice->Clear_BackBuffer_View(&vColor)))
		return E_FAIL;

	if (FAILED(m_pGraphicDevice->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	// Renderer->Render 같은 그리기 함수
	return S_OK;
}
HRESULT CGameInstance::Draw_End()
{
	if (FAILED(m_pGraphicDevice->Present()))
		return E_FAIL;

	return S_OK;
}
#pragma endregion


#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	return m_pTimerManager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	return m_pTimerManager->Add_Timer(pTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _tchar* pTimerTag)
{
	m_pTimerManager->Compute_TimeDelta(pTimerTag);
}
#pragma endregion

void CGameInstance::Free()
{
	__super::Free();

	m_pGraphicDevice->Free();
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pTimerManager);
}