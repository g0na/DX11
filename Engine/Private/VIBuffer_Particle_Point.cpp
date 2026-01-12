#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Particle{ pDevice, pContext }
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype)
    : CVIBuffer_Particle{ Prototype }
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const PARTICLE_DESC* pInitialDesc)
{
    m_iVertexStride = sizeof(VTXPOS);
    m_iNumVertices = 1;
    m_iIndexStride = 0;

    m_iNumIndices = 0;
    m_iNumVertexBuffers = 2;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

    const PARTICLE_POINT_DESC* pParticleDesc = static_cast<const PARTICLE_POINT_DESC*>(pInitialDesc);

    m_isLoop = pParticleDesc->isLoop;
    m_iInstanceVertexStride = sizeof(VTXPARTICLE);
    m_iNumInstance = pParticleDesc->iNumInstance;
    m_iIndexCountPerInstance = m_iNumIndices;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    m_pVertexPositions[0] = pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
   
    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INSTANCE_BUFFER
    
    m_InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_InstanceBufferDesc.MiscFlags = 0;
    m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

    m_pInstanceVertices = new VTXPARTICLE[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(VTXPARTICLE) * m_iNumInstance);

    m_pSpeeds = new _float[m_iNumInstance];
    ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

    m_pInitialVelocities = new _float3[m_iNumInstance];
    ZeroMemory(m_pInitialVelocities, sizeof(_float3) * m_iNumInstance);

    m_pVelocities = new _float3[m_iNumInstance];
    ZeroMemory(m_pVelocities, sizeof(_float3) * m_iNumInstance);

    m_vPivot = pParticleDesc->vPivot;

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        _float      fScale = m_pGameInstance->Random(pParticleDesc->vSize.x, pParticleDesc->vSize.y);
        m_pSpeeds[i] = m_pGameInstance->Random(pParticleDesc->vSpeed.x, pParticleDesc->vSpeed.y);

        m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
        m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
        m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
        m_pInstanceVertices[i].vTranslation = _float4(
            m_pGameInstance->Random(pParticleDesc->vCenter.x - pParticleDesc->vRange.x * 0.5f, pParticleDesc->vCenter.x + pParticleDesc->vRange.x * 0.5f),
            m_pGameInstance->Random(pParticleDesc->vCenter.y - pParticleDesc->vRange.y * 0.5f, pParticleDesc->vCenter.y + pParticleDesc->vRange.y * 0.5f),
            m_pGameInstance->Random(pParticleDesc->vCenter.z - pParticleDesc->vRange.z * 0.5f, pParticleDesc->vCenter.z + pParticleDesc->vRange.z * 0.5f),
            1.f
        );

        m_pInstanceVertices[i].vLifeTime = _float2(0.f, m_pGameInstance->Random(pParticleDesc->vLifeTime.x, pParticleDesc->vLifeTime.y));

        _float3 vDirection = _float3(m_pInstanceVertices[i].vTranslation.x - m_vPivot.x,
                                     m_pInstanceVertices[i].vTranslation.y - m_vPivot.y,
                                     m_pInstanceVertices[i].vTranslation.z - m_vPivot.z);

        XMStoreFloat3(&vDirection, XMVector3Normalize(XMLoadFloat3(&vDirection)));

        m_pInitialVelocities[i] = m_pVelocities[i] = _float3(vDirection.x * m_pSpeeds[i], 
                                                             vDirection.y * m_pSpeeds[i] + 0.05f,
                                                             vDirection.z * m_pSpeeds[i]);
        
    }   

    Safe_Delete_Array(pVertices);
#pragma endregion




    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
    D3D11_SUBRESOURCE_DATA      InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL; 

    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Bind_Resource()
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
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);



    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Render()
{
    m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

    return S_OK;
}

CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC* pInitialDesc)
{
    CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Particle_Point::Clone(void* pArg)
{
    CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{
    __super::Free();
}
