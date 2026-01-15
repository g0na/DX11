#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CModel;
NS_END

NS_BEGIN(Client)

class CBoss_Death final : public CState
{
private:
	CBoss_Death();
	CBoss_Death(const CBoss_Death& Prototype) = delete;
	virtual ~CBoss_Death() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CModel* m_pMonsterModel = { nullptr };

private:
	_float			m_fDeathTime = {};

public:
	static CBoss_Death* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END