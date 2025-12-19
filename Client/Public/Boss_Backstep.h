#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
NS_END

NS_BEGIN(Client)

class CBoss_Backstep final : public CState
{
private:
	CBoss_Backstep();
	CBoss_Backstep(const CBoss_Backstep& Prototype) = delete;
	virtual ~CBoss_Backstep() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*					m_pGameInstance = { nullptr };
	CStateMachine*					m_pStateMachine = { nullptr };

	_float							m_fTimeElapsed = {};

public:
	static CBoss_Backstep* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END