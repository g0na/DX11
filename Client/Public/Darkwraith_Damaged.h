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

class CDarkwraith_Damaged final : public CState
{
private:
	CDarkwraith_Damaged();
	CDarkwraith_Damaged(const CDarkwraith_Damaged& Prototype) = delete;
	virtual ~CDarkwraith_Damaged() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CStateMachine* m_pStateMachine = { nullptr };
	CTransform* m_pMonsterTransform = { nullptr };
	CModel* m_pMonsterModel = { nullptr };

	_float* m_pCurAngle = { nullptr };

public:
	static CDarkwraith_Damaged* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END