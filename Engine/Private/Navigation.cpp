#include "Navigation.h"
#include "Cell.h"
#include "GameInstance.h"

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

HRESULT CNavigation::Initialize_Prototype(const vector<const _tchar*>& pNavigationDatas)
{
    // TODO: 파일로부터 읽어와서 네비게이션 데이터 저장하기
    _uint iCellOffset = { 0 };

    for (auto& pNVM : pNavigationDatas)
    {
        if (FAILED(Load_NVM(pNVM, iCellOffset)))
            return E_FAIL;

        iCellOffset = (_uint)m_vecCells.size();
    }

    m_iNumCells = (_uint)m_vecCells.size();

    if (FAILED(SetUp_BorderNeighbors()))
        return E_FAIL;

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (m_pShader == nullptr)
        return E_FAIL;
#endif // _DEBUG


    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    m_iCurrentCellIndex = pDesc->iCurrentCellIndex;
    m_pParentMatrix = pDesc->pParentMatrix;

    return S_OK;
}

HRESULT CNavigation::Load_NVM(const _tchar* pNavigationData, _uint iCellOffset)
{
    ifstream fin(pNavigationData, ios::binary);
    if (fin.is_open() == false)
        return E_FAIL;

    // Header 읽기
    NVM_HEADER headerDesc{};
    fin.read(CHARCAST(&headerDesc), sizeof(NVM_HEADER));

    // Vertex 위치로 이동
    fin.seekg(headerDesc.iVertexOffset, ios::beg);

    // Vertex 읽기
    vector<_float3> vertices(headerDesc.iVertexCount);
    fin.read(CHARCAST(vertices.data()), sizeof(_float3) * headerDesc.iVertexCount);

    // Cell 위치로 이동
    fin.seekg(headerDesc.iCellOffset, ios::beg);

    // Cell 읽기 및 생성
    for (_uint i = 0; i < headerDesc.iCellCount; i++)
    {
        NVM_CELL cellDesc{};
        fin.read(CHARCAST(&cellDesc), sizeof(NVM_CELL));

        _float3 vPoints[3] = {
            vertices[cellDesc.iVertexIndices[0]],
            vertices[cellDesc.iVertexIndices[1]],
            vertices[cellDesc.iVertexIndices[2]]
        };

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, iCellOffset + i);
        if (pCell == nullptr)
        {
            fin.close();
            return E_FAIL;
        }

        // 이웃 설정
        _int iNeighbors[3] = {};
        for (_uint j = 0; j < 3; j++)
        {
            // 이웃이 있다면 Offset 만큼 더하고 없다면 -1로 설정
            iNeighbors[j] = (cellDesc.iNeighborIndices[j] >= 0) ?
                cellDesc.iNeighborIndices[j] + iCellOffset : -1;
        }

        pCell->Set_Neighbors(iNeighbors);
        m_vecCells.push_back(pCell);
    }

    fin.close();

    return S_OK;
}

_vector CNavigation::SetOn_Navigation(_fvector vWorldPos)
{
    _vector vCellPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    vCellPos = XMVectorSetY(vCellPos, m_vecCells[m_iCurrentCellIndex]->Compute_Height(vCellPos));

    return XMVector3TransformCoord(vCellPos, XMLoadFloat4x4(m_pParentMatrix));
}

_bool CNavigation::CanMove(_fvector vResultPos)
{
    if (m_iCurrentCellIndex == -1)
        return false;

    _vector vCellPos = XMVector3TransformCoord(vResultPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrix)));

    _int iNeighborIndex = { -1 };

    // 셀을 나갔다
    if (m_vecCells[m_iCurrentCellIndex]->isIn(vCellPos, &iNeighborIndex) == false)
    {
        // 이웃이 없다면
        if (iNeighborIndex == -1)
            return false;

        // 이웃이 있다면
        else
        {
            while (true)
            {
                if (m_vecCells[iNeighborIndex]->isIn(vCellPos, &iNeighborIndex))
                    break;

                if (iNeighborIndex == -1)
                    return false;
            }

            m_iCurrentCellIndex = iNeighborIndex;

            return true;
        }
    }

    return true;
}

HRESULT CNavigation::SetUp_Neighbors(const _tchar* pNavigationNeighborData)
{
    if (pNavigationNeighborData == nullptr)
    {
        for (auto& pSrcCell : m_vecCells)
        {
            for (auto& pDstCell : m_vecCells)
            {
                if (pSrcCell == pDstCell)
                    continue;

                if (pDstCell->Compare(pSrcCell->Get_Point(CELLPOINT::A), pSrcCell->Get_Point(CELLPOINT::B)) == true)
                    pSrcCell->Set_Neighbor(LINE::AB, pDstCell);

                if (pDstCell->Compare(pSrcCell->Get_Point(CELLPOINT::B), pSrcCell->Get_Point(CELLPOINT::C)) == true)
                    pSrcCell->Set_Neighbor(LINE::BC, pDstCell);

                if (pDstCell->Compare(pSrcCell->Get_Point(CELLPOINT::C), pSrcCell->Get_Point(CELLPOINT::A)) == true)
                    pSrcCell->Set_Neighbor(LINE::CA, pDstCell);
            }
        }
    }
    else
    {
        // TODO: 파일로 부터 읽어와서 이웃 설정하기
    }

    return S_OK;
}

HRESULT CNavigation::SetUp_BorderNeighbors()
{
    // 인덱스가 -1인 경계만 검사
    for (auto& pSrcCell : m_vecCells)
    {
        for (_uint i = 0; i < ENUM_TO_UINT(LINE::END); i++)
        {
            // 이웃이 있으면 패스
            if (pSrcCell->Get_Neighbor(static_cast<LINE>(i)) != -1)
                continue;

            // 이웃이 없는 라인의 두 포인트
            CELLPOINT ePoint1 = static_cast<CELLPOINT>(i);
            CELLPOINT ePoint2 = static_cast<CELLPOINT>((i + 1) % 3);

            _vector vSrc1 = pSrcCell->Get_Point(ePoint1);
            _vector vSrc2 = pSrcCell->Get_Point(ePoint2);

            // 다른 셀에서 맞는 라인 찾기
            for (auto& pDstCell : m_vecCells)
            {
                // 서로 같은 셀이면 패스
                if (pSrcCell == pDstCell)
                    continue;

                // 두 포인트와 일치하는 라인이 있다면 이웃 설정
                if (pDstCell->Compare(vSrc1, vSrc2))
                {
                    pSrcCell->Set_Neighbor(static_cast<LINE>(i), pDstCell);
                    break;
                }
            }
        }
    }

    return S_OK;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    _float4x4 WorldMatrix = *m_pParentMatrix;

    if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShader, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShader, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    _float4 vColor = {};

    if (m_iCurrentCellIndex == -1)
    {
        vColor = _float4(0.f, 1.f, 0.f, 1.f);

        if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(vColor))))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;

        m_pShader->Begin(0);

        for (auto& pCell : m_vecCells)
            pCell->Render();
    }
    else
    {
        vColor = _float4(1.f, 0.f, 0.f, 1.f);

        WorldMatrix._42 += 0.1f;

        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(vColor))))
            return E_FAIL;

        m_pShader->Begin(0);

        m_vecCells[m_iCurrentCellIndex]->Render();
    }

    return S_OK;
}
#endif // _DEBUG


CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<const _tchar*>& pNavigationData)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pNavigationData)))
    {
        MSG_BOX("Failed to Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
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