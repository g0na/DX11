#pragma once
#include "Prototype_Manager.h"

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
	void					Clear(_uint iLevelID);		// 정해진 레벨의 자원을 정리한다.
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	void		Compute_TimeDelta(const _tchar* pTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT	Change_Level(_uint iCurLevelID, class CLevel* pCurLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelNum, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelNum, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	HRESULT Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strProtoTag,
		_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
#pragma endregion

#pragma region RENDERER
public:
	HRESULT	Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pObj);
#pragma endregion

private:
	class CGraphic_Device*		m_pGraphicDevice = { nullptr };
	class CTimer_Manager*		m_pTimerManager = { nullptr };
	class CLevel_Manager*		m_pLevelManager = { nullptr };
	class CObject_Manager*		m_pObjectManager = { nullptr };
	class CPrototype_Manager*	m_pPrototypeManager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;

}; 

NS_END