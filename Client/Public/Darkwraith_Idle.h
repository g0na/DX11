#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CTransform;
NS_END

NS_BEGIN(Client)

class CDarkwraith_Idle final : public CState
{
private:
	CDarkwraith_Idle();
	CDarkwraith_Idle(const CDarkwraith_Idle& Prototype) = delete;
	virtual ~CDarkwraith_Idle() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*					m_pGameInstance = { nullptr };
	CStateMachine*					m_pStateMachine = { nullptr };
	CTransform*						m_pMonsterTransform = { nullptr };

public:
	static CDarkwraith_Idle* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END