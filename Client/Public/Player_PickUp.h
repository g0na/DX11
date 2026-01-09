#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
NS_END

NS_BEGIN(Client)

class CPlayer_PickUp final : public CState
{
private:
	CPlayer_PickUp();
	CPlayer_PickUp(const CPlayer_PickUp& Prototype) = delete;
	virtual ~CPlayer_PickUp() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CStateMachine*	m_pStateMachine = { nullptr };
	class CBody*	m_pPlayerBody = { nullptr };
	class CWeapon*	m_pWeapon = { nullptr };
	class CShield*	m_pShield = { nullptr };

private:
	_float			m_fTimeElapsed = {};

public:
	static CPlayer_PickUp* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END