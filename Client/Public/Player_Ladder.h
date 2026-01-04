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

class CPlayer_Ladder final : public CState
{
private:
	CPlayer_Ladder();
	CPlayer_Ladder(const CPlayer_Ladder& Prototype) = delete;
	virtual ~CPlayer_Ladder() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CStateMachine*	m_pStateMachine = { nullptr };
	CTransform*		m_pPlayerTransform = { nullptr };
	CModel*			m_pPlayerModel = { nullptr };
	class CBody*	m_pPlayerBody = { nullptr };

private:
	_vector			m_vInputDir = {};

public:
	static CPlayer_Ladder* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END