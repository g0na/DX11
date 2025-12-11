#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum PLAYERSTATE {
		IDLE,
		WALK,
		RUN,
		ROLL,
		GUARD,
		RECOIL,
		ATTACK,
		DAMAGED,
		DEATH,
		HEAL,
		END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	_float*				Get_CurAnglePtr() { return &m_fCurAngle; }
	const _bool			Is_Damaged() const { return m_bIsDamaged; }
	void				Set_Damaged(_bool isDamaged) { m_pStateMachine->Set_BoolData(TEXT("Player_Damaged"), isDamaged); }

public:
	virtual HRESULT Initialize_Prototype() override;		// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg) override;							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOtherObject) override;
	virtual void OnCollisionExit(CGameObject* pOtherObject) override;

private:	
	class CBody*	m_pBody = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CGameObject*	m_pCollidingObject = { nullptr };

	_vector			m_vPrevPosition = {};

	_float			m_fCurAngle = { 0.f };
	
	_bool			m_bIsDamaged = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_PartObjects();
	HRESULT Ready_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END