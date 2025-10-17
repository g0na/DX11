#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float	fSpeedPerSec = {};
		_float	fRotationPerSec = {};
	}TRANSFORM_DESC;

private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	_float3 Get_Scale() const
	{
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
					   XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
					   XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK))));
	}

	_vector	Get_State(STATE eState) const 
	{
		return XMLoadFloat4x4(&m_WorldMatrix).r[ENUM_TO_UINT(eState)];
	}

	void Set_State(STATE eState, const _float4& vState)
	{
		memcpy(&m_WorldMatrix.m[ENUM_TO_UINT(eState)], &vState, sizeof(_float4));
	}

	void Set_State(STATE eState, _fvector vState)
	{
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_TO_UINT(eState)]), vState);
	}

	void Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
	{
		// Right, Up, Look 벡터의 크기를 바꿔준다.
		Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
		Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fScaleY);
		Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;

public:
	// 셰이더에게 월드 행렬을 던져주는 함수
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);

	// 임의의 축 회전
	void Rotation(_fvector vAxis, _float fRadian);
	// 쿼터니언 회전
	void Rotation(_float fRadianX, _float fRadianY, _float fRadianZ);
	// 대상을 바라보는 회전
	void Turn(_fvector fAxis, _float fTimeDelta);

	void LookAt(_vector vTargetPos);
	void Chase(_vector vTargetPos, _float fTimeDelta, _float fMinDist);

private:
	_float		m_fSpeedPerSec = {};
	_float		m_fRotationPerSec = {};
	_float4x4	m_WorldMatrix = {};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END