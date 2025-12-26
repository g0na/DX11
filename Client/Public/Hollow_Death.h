#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CHollow_Death final : public CState
{
private:
	CHollow_Death();
	CHollow_Death(const CHollow_Death& Prototype) = delete;
	virtual ~CHollow_Death() = default;

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
	static CHollow_Death* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END