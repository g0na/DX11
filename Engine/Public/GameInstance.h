#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final :  public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region ENGINE
public:
	HRESULT					Initialize_Engine();
	HRESULT					Initialize_Device(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContextOut);
	void					Update_Engine(const _float& fTimeDelta);
	HRESULT					Draw();
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	void		Compute_TimeDelta(const _tchar* pTimerTag);
#pragma endregion

private:
	class CGraphic_Device*	m_pGraphicDevice = { nullptr };
	class CTimerManager*	m_pTimerManager = { nullptr };

public:
	virtual void Free() override;

}; 

NS_END