#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CState abstract : public CBase
{
protected:
	CState();
	CState(const CState& Prototype) = delete;
	virtual ~CState() = default;

public:
	class CGameObject* Get_Owner() { return m_pOwner; }
	_uint Get_StateType() { return m_iStateType; }

	void Set_StateType(_uint iStateType) { m_iStateType = iStateType; }

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);
	
	virtual void Enter_State() = 0;
	virtual void Update_State(_float fTimeDelta) = 0;
	virtual void Exit_State() = 0;

protected:
	class CGameObject*	m_pOwner = { nullptr };		// 상태의 소유자
	_uint				m_iStateType = {};			// 상태 타입

public:
	virtual void Free() override;
};

NS_END