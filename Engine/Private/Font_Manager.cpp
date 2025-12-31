#include "Font_Manager.h"

#include "Font.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Initialize()
{
    m_pBatch = new SpriteBatch(m_pContext);

    return S_OK;
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    CFont* pFont = CFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
    CFont*  pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    m_pBatch->Begin();

    pFont->Draw_Text(m_pBatch, pText, vPosition, vColor);    

    m_pBatch->End();

    return S_OK;
}

CFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Fonts.find(strFontTag);

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;    
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

    /* 메인앱을 완벽히 사용하기 위한 기타 초기화작업을 수행한다. */
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CFont_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CFont_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);
    m_Fonts.clear();

    Safe_Delete(m_pBatch);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
