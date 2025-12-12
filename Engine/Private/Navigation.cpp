#include "Navigation.h"
#include "Cell.h"

const _float4x4* CNavigation::m_pParentMatrix = { nullptr };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
    : CComponent { Prototype }
    , m_vecCells { Prototype.m_vecCells }
    , m_iNumCells { Prototype.m_iNumCells }
    , m_iCurrentCellIndex { Prototype.m_iCurrentCellIndex }
#ifdef _DEBUG
    , m_pShader { Prototype.m_pShader }
#endif // _DEBUG
{
#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif // _DEBUG

    for (auto& pCell : m_vecCells)
        Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationData, const _tchar* pNavigationNeighborData)
{
    return E_NOTIMPL;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    return E_NOTIMPL;
}

_vector CNavigation::SetOn_Navigation(_fvector vWorldPos)
{
    return _vector();
}

_bool CNavigation::CanMove(_fvector vResultPos)
{
    return _bool();
}

HRESULT CNavigation::SetUp_Neighbors(const _tchar* pNavigationNeighborData)
{
    return E_NOTIMPL;
}

HRESULT CNavigation::Render()
{
    return E_NOTIMPL;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationData, const _tchar* pNavigationNeighborData)
{
    return nullptr;
}

CComponent* CNavigation::Clone(void* pArg)
{
    return nullptr;
}

void CNavigation::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif // _DEBUG

    for (auto& pCell : m_vecCells)
        Safe_Release(pCell);
    m_vecCells.clear();
}