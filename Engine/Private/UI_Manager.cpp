#include "UI_Manager.h"

CUI_Manager::CUI_Manager()
{
}

HRESULT CUI_Manager::Add_UI(_wstring strUITag, CUIObj* pUIObj)
{
    if (Find_UI(strUITag) != nullptr || pUIObj == nullptr)
        return E_FAIL;

    m_mapUIs.emplace(strUITag, pUIObj);
    Safe_AddRef(pUIObj);

    return S_OK;
}

CUIObj* CUI_Manager::Find_UI(_wstring strUITag)
{
    auto iter = m_mapUIs.find(strUITag);
    if (iter == m_mapUIs.end())
        return nullptr;

    return iter->second;
}

void CUI_Manager::Show_UI(_wstring strUITag)
{
    auto iter = m_mapUIs.find(strUITag);
    if (iter == m_mapUIs.end())
        return;

    iter->second->Set_Visible(true);
}

void CUI_Manager::Hide_UI(_wstring strUITag)
{
    auto iter = m_mapUIs.find(strUITag);
    if (iter == m_mapUIs.end())
        return;

    iter->second->Set_Visible(false);
}

CUI_Manager* CUI_Manager::Create()
{
    return new CUI_Manager();
}

void CUI_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_mapUIs)
        Safe_Release(Pair.second);
    m_mapUIs.clear();
}
