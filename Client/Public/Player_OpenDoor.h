#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
NS_END

NS_BEGIN(Client)

class CPlayer_OpenDoor final : public CState
{
private:
	CPlayer_OpenDoor();
	CPlayer_OpenDoor(const CPlayer_OpenDoor& Prototype) = delete;
	virtual ~CPlayer_OpenDoor() = default;

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
	static CPlayer_OpenDoor* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END