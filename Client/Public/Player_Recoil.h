#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CStateMachine;
NS_END

NS_BEGIN(Client)

class CPlayer_Recoil final : public CState
{
private:
	CPlayer_Recoil();
	CPlayer_Recoil(const CPlayer_Recoil& Prototype) = delete;
	virtual ~CPlayer_Recoil() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CStateMachine* m_pStateMachine = { nullptr };
	class CBody* m_pPlayerBody = { nullptr };

public:
	static CPlayer_Recoil* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END