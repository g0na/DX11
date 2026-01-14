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

class CBoss_FlyAttack final : public CState
{
private:
	CBoss_FlyAttack();
	CBoss_FlyAttack(const CBoss_FlyAttack& Prototype) = delete;
	virtual ~CBoss_FlyAttack() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*					m_pGameInstance = { nullptr };
	CStateMachine*					m_pStateMachine = { nullptr };
	CTransform*						m_pMonsterTransform = { nullptr };
	CModel*							m_pMonsterModel = { nullptr };
	class CMonster_Boss*			m_pBoss = { nullptr };
	class CFlyCollider_Boss*		m_pFlyCollider = { nullptr };

	_float* m_pCurAngle = { nullptr };
	_bool	m_bDustFlag = {};
	_float	m_fFlyAttackDelay = {};

public:
	static CBoss_FlyAttack* Create(class CGameObject* pOwner);
	virtual void Free() override;
};

NS_END