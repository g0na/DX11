#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
NS_END

NS_BEGIN(Client)

class CPlayer_Healing final : public CState
{
private:
	CPlayer_Healing();
	CPlayer_Healing(const CPlayer_Healing& Prototype) = delete;
	virtual ~CPlayer_Healing() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CStateMachine*	m_pStateMachine = { nullptr };
	class CBody*	m_pPlayerBody = { nullptr };
	class CEstus*	m_pEstus = { nullptr };
	class CWeapon*	m_pWeapon = { nullptr };

	_float			m_fCoolDown = {};

public:
	static CPlayer_Healing* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END