#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CTransform;
NS_END

NS_BEGIN(Client)

class CPlayer_Attack final : public CState
{
private:
	CPlayer_Attack();
	CPlayer_Attack(const CPlayer_Attack& Prototype) = delete;
	virtual ~CPlayer_Attack() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CStateMachine* m_pStateMachine = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	class CBody* m_pPlayerBody = { nullptr };

	_vector			m_vInputDir = {};
	_float*			m_pCurAngle = { nullptr };

	_uint			m_iAttackCnt = {};
	_float			m_fAttackDelay = {};

public:
	static CPlayer_Attack* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END