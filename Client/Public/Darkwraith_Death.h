#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CDarkwraith_Death final : public CState
{
private:
	CDarkwraith_Death();
	CDarkwraith_Death(const CDarkwraith_Death& Prototype) = delete;
	virtual ~CDarkwraith_Death() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };

private:
	_float			m_fDeathTime = {};

public:
	static CDarkwraith_Death* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END