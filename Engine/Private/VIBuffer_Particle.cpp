#include "VIBuffer_Particle.h"

CVIBuffer_Particle::CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Particle::CVIBuffer_Particle(const CVIBuffer_Particle& Prototype)
	: CVIBuffer{ Prototype }
	, m_pVBInstance { Prototype.m_pVBInstance }
	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_iIndexCountPerInstance{ Prototype.m_iIndexCountPerInstance }
	, m_iNumInstance { Prototype.m_iNumInstance }
	, m_iInstanceVertexStride{ Prototype.m_iInstanceVertexStride }
	, m_pSpeeds { Prototype.m_pSpeeds }
	, m_pVelocities	{ Prototype.m_pVelocities }
	, m_pInitialVelocities { Prototype.m_pInitialVelocities }
	, m_isLoop { Prototype.m_isLoop }
	, m_vPivot { Prototype.m_vPivot }
{
	Safe_AddRef(m_pVBInstance);
}

_bool CVIBuffer_Particle::Get_IsFinished()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
		{
			m_pContext->Unmap(m_pVBInstance, 0);
			return false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return true;
}

HRESULT CVIBuffer_Particle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Particle::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Particle::Bind_Resource()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint		 iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint		 iOffsets[] = {
		0, 
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);


	

	return S_OK;
}

HRESULT CVIBuffer_Particle::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Particle::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE*	pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
		pVertices[i].vLifeTime.x += fTimeDelta;
		if (true == m_isLoop && pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{	
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		/*pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;*/
		_vector		vLook = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f)) * m_pSpeeds[i];

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vLook * fTimeDelta);

		pVertices[i].vLifeTime.x += fTimeDelta;
		if (true == m_isLoop && pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::BloodDrop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		m_pVelocities[i].y -= 9.8f * fTimeDelta;
		pVertices[i].vTranslation.x += m_pVelocities[i].x * fTimeDelta;
		pVertices[i].vTranslation.y += m_pVelocities[i].y * fTimeDelta;
		pVertices[i].vTranslation.z += m_pVelocities[i].z * fTimeDelta;

		pVertices[i].vLifeTime.x += fTimeDelta;
		if (true == m_isLoop && pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Reset()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;

		m_pVelocities[i] = m_pInitialVelocities[i];

		pVertices[i].vLifeTime.x = 0.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Reset(const _float3& vDirection)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;

		//m_pVelocities[i] = m_pInitialVelocities[i];
		_float3 vFinalDir = _float3(vDirection.x + m_pInitialVelocities[i].x,
									vDirection.y + m_pInitialVelocities[i].y,
									vDirection.z + m_pInitialVelocities[i].z);
		XMStoreFloat3(&vFinalDir, XMVector3Normalize(XMLoadFloat3(&vFinalDir)));

		m_pVelocities[i].x = vFinalDir.x * m_pSpeeds[i];
		m_pVelocities[i].y = vFinalDir.y * m_pSpeeds[i] + 0.05f;
		m_pVelocities[i].z = vFinalDir.z * m_pSpeeds[i];

		pVertices[i].vLifeTime.x = 0.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Free()
{
	__super::Free();

	if (false == m_bIsClone)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pVelocities);
		Safe_Delete_Array(m_pInitialVelocities);
		Safe_Delete_Array(m_pInstanceVertices);
	}

	Safe_Release(m_pVBInstance);
}
