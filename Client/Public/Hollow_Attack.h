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

class CHollow_Attack final : public CState
{
private:
	CHollow_Attack();
	CHollow_Attack(const CHollow_Attack& Prototype) = delete;
	virtual ~CHollow_Attack() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*		m_pGameInstance = { nullptr };
	CStateMachine*		m_pStateMachine = { nullptr };
	CTransform*			m_pMonsterTransform = { nullptr };
	CModel*				m_pMonsterModel = { nullptr };
	class CWeapon*		m_pMonsterWeapon = { nullptr };

	_uint	m_iAttackCnt = {};
	_float* m_pCurAngle = { nullptr };
	_float	m_fAttackDelay = {};

public:
	static CHollow_Attack* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END