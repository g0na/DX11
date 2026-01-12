#pragma once

#include "VIBuffer_Particle.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Point final : public CVIBuffer_Particle
{
public:
	typedef struct tagVIBuffer_Particle_Point_Desc : public CVIBuffer_Particle::PARTICLE_DESC
	{

	}PARTICLE_POINT_DESC;
private:
	CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype);
	virtual ~CVIBuffer_Particle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(const PARTICLE_DESC* pInitialDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resource() override;
	virtual HRESULT Render() override;
public:
	static CVIBuffer_Particle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC* pInitialDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END