#include "ContainerObject.h"
#include "PartObject.h"
#include "GameInstance.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CContainerObject::Update_Priority(_float fTimeDelta)
{
	for (auto& Pair : m_mapPartObjects)
	{
		if (Pair.second->Get_IsActive())
			Pair.second->Update_Priority(fTimeDelta);
	}
}

void CContainerObject::Update(_float fTimeDelta)
{
	for (auto& Pair : m_mapPartObjects)
	{
		if (Pair.second->Get_IsActive())
			Pair.second->Update(fTimeDelta);
	}
}

void CContainerObject::Update_Late(_float fTimeDelta)
{
	for (auto& Pair : m_mapPartObjects)
	{
		if (Pair.second->Get_IsActive())
			Pair.second->Update_Late(fTimeDelta);
	}
}

HRESULT CContainerObject::Render()
{
	return S_OK;
}

HRESULT CContainerObject::Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartObjTag, void* pArg)
{
	// nullptr 이라면 이미 PartObject가 있다는 것
	if (Find_PartObject(strPartObjTag) != nullptr)
		return E_FAIL;

	CPartObject* pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (pPartObject == nullptr)
		return E_FAIL;

	m_mapPartObjects.emplace(strPartObjTag, pPartObject);
	m_listPartObjects.push_back(pPartObject);

	return S_OK;
}

CPartObject* CContainerObject::Find_PartObject(const _wstring& strPartTag)
{
	auto iter = m_mapPartObjects.find(strPartTag);
	if (iter == m_mapPartObjects.end())
		return nullptr;

	return iter->second;
}

void CContainerObject::Free()
{
	__super::Free();

	for (auto& Pair : m_mapPartObjects)
		Safe_Release(Pair.second);
	m_mapPartObjects.clear();
}
