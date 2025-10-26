#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex)
{
	if (iIndex >= m_listLights.size())
		return nullptr; 

	auto iter = m_listLights.begin();

	for (_uint i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
	CLight* pLight = CLight::Create(LightDesc);

	if (pLight == nullptr)
		return E_FAIL;

	m_listLights.push_back(pLight);

	return S_OK;
}

CLight_Manager* CLight_Manager::Create()
{
	return new CLight_Manager();
}

void CLight_Manager::Free()
{
	__super::Free();

	// 리스트 반환
	for (auto& pLights : m_listLights)
		Safe_Release(pLights);

	m_listLights.clear();
}
