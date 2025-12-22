#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CPlayer_Death final : public CState
{
private:
	CPlayer_Death();
	CPlayer_Death(const CPlayer_Death& Prototype) = delete;
	virtual ~CPlayer_Death() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	class CBody*	m_pPlayerBody = { nullptr };

public:
	static CPlayer_Death* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END