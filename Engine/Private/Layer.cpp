#include "Layer.h"
#include "GameObject.h"

USING(Engine)

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pObj)
{
	m_listObject.push_back(pObj);
	return S_OK;
}

void CLayer::Update_Priority(_float fTimeDelta)
{
	for (auto& pGameObject : m_listObject)
	{
		if (pGameObject != nullptr)
			pGameObject->Update_Priority(fTimeDelta);
	}
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_listObject)
	{
		if (pGameObject != nullptr)
			pGameObject->Update(fTimeDelta);
	}
}

void CLayer::Update_Late(_float fTimeDelta)
{
	for (auto& pGameObject : m_listObject)
	{
		if (pGameObject != nullptr)
			pGameObject->Update_Late(fTimeDelta);
	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_listObject)
		Safe_Release(pGameObject);

	m_listObject.clear();
}