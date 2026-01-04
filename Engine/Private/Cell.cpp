#include "Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
    memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_TO_UINT(CELLPOINT::END));

    m_iIndex = iIndex;

    _vector vLine = {};

    vLine = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::B)]) - XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::A)]), 0.f));
    m_vNormals[ENUM_TO_UINT(LINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::C)]) - XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::B)]), 0.f));
    m_vNormals[ENUM_TO_UINT(LINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::A)]) - XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::C)]), 0.f));
    m_vNormals[ENUM_TO_UINT(LINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
    if (m_pVIBuffer == nullptr)
        return E_FAIL;
#endif // _DEBUG

    return S_OK;
}

_bool CCell::Compare(_fvector vSrcPoint, _fvector vDstPoint)
{
    _vector vEpsilon = XMVectorSet(0.01f, 0.01f, 0.01f, 0.01f);

    if (XMVector3NearEqual(vSrcPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::A)]), vEpsilon) == true)
    {
        if (XMVector3NearEqual(vDstPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::B)]), vEpsilon) == true)
            return true;

        if (XMVector3NearEqual(vDstPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::C)]), vEpsilon) == true)
            return true;
    }

    if (XMVector3NearEqual(vSrcPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::B)]), vEpsilon) == true)
    {
        if (XMVector3NearEqual(vDstPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::C)]), vEpsilon) == true)
            return true;

        if (XMVector3NearEqual(vDstPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::A)]), vEpsilon) == true)
            return true;
    }

    if (XMVector3NearEqual(vSrcPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::C)]), vEpsilon) == true)
    {
        if (XMVector3NearEqual(vDstPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::A)]), vEpsilon) == true)
            return true;

        if (XMVector3NearEqual(vDstPoint, XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::B)]), vEpsilon) == true)
            return true;
    }

    return false;
}

_bool CCell::isIn(_fvector vResultPos, _int* pNeighborIndex, LINE* pLine)
{
    for (_uint i = 0; i < ENUM_TO_UINT(LINE::END); i++)
    {
        _vector vDir = XMVector3Normalize(vResultPos - XMLoadFloat3(&m_vPoints[i]));

        if (XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i]))) > 0.f)
        {
            *pNeighborIndex = m_iNeighbors[i];
            *pLine = static_cast<LINE>(i);

            return false;
        }
    }

    return true;
}

_float CCell::Compute_Height(_vector vCellPos)
{
    _vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::A)]),
        XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::B)]),
        XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(CELLPOINT::C)]));

    // ax + by + cz + d = 0
    // y = (-ax -cz -d) / b

    return (-XMVectorGetX(vPlane) * XMVectorGetX(vCellPos) - XMVectorGetZ(vPlane) * XMVectorGetZ(vCellPos) - XMVectorGetW(vPlane)) 
        / XMVectorGetY(vPlane);
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    m_pVIBuffer->Render();

    return S_OK;
}
#endif


CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
    CCell* pInstance = new CCell(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX("Failed to Created : CCell");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCell::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif // _DEBUG

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
