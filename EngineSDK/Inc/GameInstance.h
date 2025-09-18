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
	HRESULT					Initialize_Engine(EngineDesc& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext);
	void					Update_Engine(const _float& fTimeDelta);
	HRESULT					Draw_Begin(const _float4& vColor);
	HRESULT					Draw();
	HRESULT					Draw_End();
	void					Clear(_uint iLevelID);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	void		Compute_TimeDelta(const _tchar* pTimerTag);
#pragma endregion

#pragma region LEVEL
public:
	HRESULT	Change_Level(_uint iCurLevelID, class CLevel* pCurLevel);
#pragma endregion


private:
	class CGraphic_Device*	m_pGraphicDevice = { nullptr };
	class CTimerManager*	m_pTimerManager = { nullptr };
	class CLevelManager*	m_pLevelManager = { nullptr };

public:
	virtual void Free() override;

}; 

NS_END