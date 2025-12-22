#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
NS_END

NS_BEGIN(Client)

class CPlayer_HealEnd final : public CState
{
private:
	CPlayer_HealEnd();
	CPlayer_HealEnd(const CPlayer_HealEnd& Prototype) = delete;
	virtual ~CPlayer_HealEnd() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CStateMachine*	m_pStateMachine = { nullptr };
	class CBody*	m_pPlayerBody = { nullptr };

public:
	static CPlayer_HealEnd* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END