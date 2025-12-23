#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CTransform;
NS_END

NS_BEGIN(Client)

class CBoss_Walk final : public CState
{
private:
	CBoss_Walk();
	CBoss_Walk(const CBoss_Walk& Prototype) = delete;
	virtual ~CBoss_Walk() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CStateMachine* m_pStateMachine = { nullptr };
	CTransform* m_pMonsterTransform = { nullptr };

private:
	_float*		m_pCurAngle = { nullptr };
	_float		m_fCoolDown = {};

public:
	static CBoss_Walk* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END