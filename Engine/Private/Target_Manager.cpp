#include "Target_Manager.h"
#include "RenderTarget.h"
#include "Shader.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (Find_RenderTarget(strTargetTag) != nullptr)
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
    if (pRenderTarget == nullptr)
        return E_FAIL;

    m_mapRenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (pRenderTarget == nullptr)
        return E_FAIL;

    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
    if (pMRTList == nullptr)
    {
        list<CRenderTarget*>    MRTList;

        MRTList.push_back(pRenderTarget);

        m_mapMRTs.emplace(strMRTTag, MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);
    Safe_AddRef(pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag)
{
    // 내가 지정한 MRT 안에 있는 타겟들을 순서대로 장치에 동시에 바인딩한다.
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
    if (pMRTList == nullptr)
        return E_FAIL;

    // 현재 설정된 화면과 깊이 버퍼를 백업하고 나중에 돌려놓는다.
    m_pContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pDSV);

    ID3D11RenderTargetView* pRTVs[8] = { nullptr };
    _uint iNumRenderTargets = {};

    // MRT 그룹의 모든 렌더 타겟을 Clear 하고 수집
    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }

    ID3D11ShaderResourceView* nullSRV[8] = { nullptr };
    m_pContext->VSSetShaderResources(0, 8, nullSRV);
    m_pContext->PSSetShaderResources(0, 8, nullSRV);
    
    // 픽셀 셰이더의 출력이 pRTVs 배열의 렌더 타겟들로 동시에 기록된다.
    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pDSV);

    return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
    // MRT 사용을 종료하고 원래 화면 (백 버퍼)로 복원한다.
    ID3D11RenderTargetView* pRenderTargets[8] = {
        m_pBackBuffer
    };

    m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pDSV);

    Safe_Release(m_pBackBuffer);
    Safe_Release(m_pDSV);

    return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (pRenderTarget == nullptr)
        return E_FAIL;

    return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring strTargetTag)
{
    auto iter = m_mapRenderTargets.find(strTargetTag);
    if (iter == m_mapRenderTargets.end())
        return nullptr;

    return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring strMRTTag)
{
    auto iter = m_mapMRTs.find(strMRTTag);
    if (iter == m_mapMRTs.end())
        return nullptr;

    return &iter->second;
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (pRenderTarget == nullptr)
        return E_FAIL;

    pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);

    return S_OK;
}

HRESULT CTarget_Manager::Render(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
    if (pMRTList == nullptr)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Render(pShader, pVIBuffer);
    }

    return S_OK;
}
#endif // _DEBUG

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CTarget_Manager(pDevice, pContext);
}

void CTarget_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_mapRenderTargets)
        Safe_Release(Pair.second);
    m_mapRenderTargets.clear();

    for (auto& Pair : m_mapMRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            Safe_Release(pRenderTarget);
        Pair.second.clear();
    }
    m_mapMRTs.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
