#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CMonster_Darkwraith final : public CGameObject
{
public:
	enum DARKWRAITHSTATE
	{
		IDLE,
		WALK,
		DAMAGED,
		DEATH,
		ATTACK,
		END
	};

private:
	CMonster_Darkwraith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Darkwraith(const CMonster_Darkwraith& Prototype);
	virtual ~CMonster_Darkwraith() = default;

public:
	const _vector Get_PlayerPos() const { return m_vPlayerPos; }
	const _float Get_TargetDist() const { return m_fDistance; }
	_float* Get_CurAnglePtr() { return &m_fCurAngle; }

	const _bool Is_Targeting() const { return m_bIsTargeting; }
	const _bool Is_AttackReady() const { return m_bAttackReady; }

	void Set_Animation(_uint iAnimationIndex, _bool isLoop);

public:
	virtual HRESULT Initialize_Prototype() override;							// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg) override;							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOtherObject) override;
	virtual void OnCollisionExit(CGameObject* pOtherObject) override;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
	CPlayer*			m_pPlayer = { nullptr };
	
	_vector				m_vPlayerPos = {};
	_bool				m_bIsTargeting = {};
	_bool				m_bAttackReady = {};
	_float				m_fCurAngle = {};
	_float				m_fDistance = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Bind_ShaderResources();

public:
	static CMonster_Darkwraith* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END