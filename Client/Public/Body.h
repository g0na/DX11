#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CAnimation;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CBody final : public CPartObject
{
public:
	typedef struct tagBodyDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
	}BODY_DESC;

private:
	CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody(const CBody& Prototype);
	virtual ~CBody() = default;

public:
	const _float4x4*		Get_SocketMatrix(const _char* pBoneName);
	const _vector			Get_WorldDelta() const { return m_vWorldDelta; }
	const _float			Get_CurAngle() const { return m_fCurAngle; }
	const _bool				Get_IsAnimFinish() const;

	void Set_PlayerTransform(CTransform* pPlayerTransform) { m_pPlayerTransform = pPlayerTransform; }
	void Set_PlayerNavigation(CNavigation* pPlayerNavigation) { m_pPlayerNavigation = pPlayerNavigation; }
	void Set_RotationAngle(_float fRotationAngle) { m_fTargetAngle = fRotationAngle; }
	void Set_Animation(_uint iAnimationIndex, _bool isLoop);
	void Set_InputDir(_vector vInputDir) { m_vInputDir = vInputDir; }
	void Set_IsOnLadder(_bool bIsOnLadder) { m_bIsOnLadder = bIsOnLadder; }

public:
	virtual HRESULT Initialize_Prototype() override;		// 원형 객체를 생성할 때 호출되는 함수. 무거운 초기화 작업(서버 패킷, 파일 입출력)을 담당한다.
	virtual HRESULT Initialize(void* pArg) override;							// 원형 객체를 복제할 때 호출되는 함수. 추가적인 초기화를 위해서
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CTransform*		m_pPlayerTransform = { nullptr };
	CNavigation*	m_pPlayerNavigation = { nullptr };

private:
	const _uint*					m_pParentState = { nullptr };
	_vector							m_vWorldDelta = {};			// 루트 모션의 최종 델타값
	_vector							m_vInputDir = {};

	_float3							m_vRotationAngle = {};
	_float							m_fRotationSpeed = {};
	_float							m_fCurAngle = {};			// 현재 각도
	_float							m_fTargetAngle = {};		// 입력 받은 각도

	_bool							m_bIsAnimFinish = { false };
	_bool							m_bIsOnLadder = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END