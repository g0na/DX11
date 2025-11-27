#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CStateMachine;
class CTransform;
NS_END

NS_BEGIN(Client)

class CPlayer_Idle final : public CState
{
private:
	CPlayer_Idle();
	CPlayer_Idle(const CPlayer_Idle& Prototype) = delete;
	virtual ~CPlayer_Idle() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CStateMachine*	m_pStateMachine = { nullptr };
	CTransform*		m_pPlayerTransform = { nullptr };
	class CBody*	m_pPlayerBody = { nullptr };

public:
	static CPlayer_Idle* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END