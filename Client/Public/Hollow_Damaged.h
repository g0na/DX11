#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CTransform;
NS_END

NS_BEGIN(Client)

class CHollow_Damaged final : public CState
{
private:
	CHollow_Damaged();
	CHollow_Damaged(const CHollow_Damaged& Prototype) = delete;
	virtual ~CHollow_Damaged() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CStateMachine* m_pStateMachine = { nullptr };
	CTransform* m_pMonsterTransform = { nullptr };

	_float* m_pCurAngle = { nullptr };

public:
	static CHollow_Damaged* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END