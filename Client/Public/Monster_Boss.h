#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CMonster_Boss final : public CContainerObject
{
public:
	enum BOSSSTATE
	{
		IDLE,
		WALK,
		BACKSTEP,
		DEATH,
		ATTACK,		// 4 콤보까지 있다.
		DASHATTACK,
		FLYATTACK,
		END
	};

private:
	CMonster_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Boss(const CMonster_Boss& Prototype);
	virtual ~CMonster_Boss() = default;

public:
	const _float4x4*	Get_SocketMatrix(const _char* pBoneName);
	const _vector		Get_PlayerPos() const { return m_vPlayerPos; }
	const _float		Get_TargetDist() const { return m_fDistance; }
	_float*				Get_CurAnglePtr() { return &m_fCurAngle; }
	_float				Get_HpRatio() { return m_fHpRatio; }
	
	void				Set_Animation(_uint iAnimationIndex, _bool isLoop);
	void				Set_Damaged(_uint iDamage);

public:
	virtual HRESULT Initialize_Prototype() override;							// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg) override;							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	void Play_Dust(_fvector vPosition);
	void Play_Blood(_fvector vPosition);

public:
	virtual void OnCollisionEnter(CGameObject* pOtherObject) override;
	virtual void OnCollisionExit(CGameObject* pOtherObject) override;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CCollider*			m_pColliderBody = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };
	CPlayer*			m_pPlayer = { nullptr };
	
	_vector				m_vPlayerPos = {};

	_bool				m_bIsTargeting = {};
	_bool				m_bAttackReady = {};
	_bool				m_bIsDamaged = {};

	// 체력 관련
	_float				m_fMaxHp = {};
	_float				m_fCurHp = {};
	_float				m_fHpRatio = {};

	_float				m_fCurAngle = {};
	_float				m_fDistance = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	static CMonster_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END