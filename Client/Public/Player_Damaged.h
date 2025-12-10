#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CTransform;
NS_END

NS_BEGIN(Client)

class CPlayer_Damaged final : public CState
{
private:
	CPlayer_Damaged();
	CPlayer_Damaged(const CPlayer_Damaged& Prototype) = delete;
	virtual ~CPlayer_Damaged() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CStateMachine*	m_pStateMachine = { nullptr };
	class CBody*	m_pPlayerBody = { nullptr };

	_float* m_pCurAngle = { nullptr };

public:
	static CPlayer_Damaged* Create(class CGameObject* pOwner, class CBody* pBody);
	virtual void Free() override;
};

NS_END