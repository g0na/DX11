#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle abstract : public CVIBuffer
{
public:	
	typedef struct tagVIBuffer_Particle_Desc
	{
		_uint		iNumInstance = {};
		_float2		vSize = {};
		_float3		vCenter = {};
		_float3		vPivot = {};
		_float3		vRange = {};
		_float2		vSpeed = {};
		_float2		vLifeTime = {};
		_bool		isLoop = {};
	}PARTICLE_DESC;

protected:
	CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle(const CVIBuffer_Particle& Prototype);
	virtual ~CVIBuffer_Particle() = default;

public:
	_bool	Get_IsFinished();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resource() override;
	virtual HRESULT Render() override;

public:
	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);
	void BloodDrop(_float fTimeDelta);

	void Reset();
	void Reset(const _float3& vDirection);

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC	m_InstanceBufferDesc = {};

protected:
	VTXPARTICLE* m_pInstanceVertices = { nullptr };
	_uint						m_iIndexCountPerInstance = {};
	_uint						m_iNumInstance = {};
	_uint						m_iInstanceVertexStride = {};
	_float*						m_pSpeeds = { nullptr };
	_float3*					m_pVelocities = { nullptr };
	_float3*					m_pInitialVelocities = { nullptr };
	_bool						m_isLoop = { false };
	_float3						m_vPivot = {};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};


NS_END