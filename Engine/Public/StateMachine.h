#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CState;

class ENGINE_DLL CStateMachine final : public CComponent
{
private:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& Prototype);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update_State(_float fTimeDelta);

public:
	// 상태 추가
	HRESULT Add_State(_uint iStateType, CState* pState);
	// 상태 전환
	HRESULT Change_State(_uint iStateType);
	// 초기 세팅용
	void Set_State(_uint iStateType);

private:
	map<_uint, CState*> m_mapStates;

	CState* m_pCurState = { nullptr };
	CState* m_pPrevState = { nullptr };

	_uint m_iCurStateType = {};
	_uint m_iPrevStateType = {};

public:
	static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END