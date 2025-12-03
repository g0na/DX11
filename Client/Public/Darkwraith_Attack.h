#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CTransform;
class CModel;
NS_END

NS_BEGIN(Client)

class CDarkwraith_Attack final : public CState
{
private:
	CDarkwraith_Attack();
	CDarkwraith_Attack(const CDarkwraith_Attack& Prototype) = delete;
	virtual ~CDarkwraith_Attack() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CStateMachine* m_pStateMachine = { nullptr };
	CTransform* m_pMonsterTransform = { nullptr };

	_float m_fAttackDelay = {};

public:
	static CDarkwraith_Attack* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END