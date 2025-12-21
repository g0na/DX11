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

class CPlayer_Roll final : public CState
{
private:
	CPlayer_Roll();
	CPlayer_Roll(const CPlayer_Roll& Prototype) = delete;
	virtual ~CPlayer_Roll() = default;

public:
	virtual HRESULT Initialize(class CGameObject* pOwner, class CBody* pBody);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	CGameInstance*		m_pGameInstance = { nullptr };
	CStateMachine*		m_pStateMachine = { nullptr };
	CTransform*			m_pPlayerTransform = { nullptr };
	CModel*				m_pPlayerModel = { nullptr };
	class CBody*		m_pPlayerBody = { nullptr };
	class CCamera_Free* m_pPlayerCamera = { nullptr };

	_vector			m_vInputDir = {};
	_float*			m_pCurAngle = { nullptr };
	_bool			m_CanRoll = {};
	_float			m_fCoolDown = {};

public:
	static CPlayer_Roll* Create(class CGameObject* pOwner, CBody* pBody);
	virtual void Free() override;
};

NS_END