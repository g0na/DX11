#include "Object_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Prototype_Manager.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iLevelNum)
{
	m_pLayers = new map<const _wstring, class CLayer*>[iLevelNum];

	m_iLevelNum = iLevelNum;

	return S_OK;
}

void CObject_Manager::Update_Priority(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevelNum; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update_Priority(fTimeDelta);
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevelNum; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Update_Late(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevelNum; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update_Late(fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint iLevelNum)
{
	// 특정 레벨의 여러 레이어들을 순회한다.
	for (auto& Pair : m_pLayers[iLevelNum])
		// 레이어 내부의 오브젝트들을 순회하며 지운다.
		Safe_Release(Pair.second);

	m_pLayers[iLevelNum].clear();
}

HRESULT CObject_Manager::Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strProtoTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	// 원본들을 담고 있는 프로토타입 매니저로부터 원본을 복제해서 가져온다.
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevelIndex, strProtoTag, pArg));
	if (pGameObject == nullptr)
		return E_FAIL;

	// 삽입하려는 레이어 그룹을 먼저 검색하고,
	// 해당 레이어 그룹이 없다면 레이어를 생성해서 오브젝트 추가.
	// 해당 레이어 그룹이 있다면 바로 오브젝트 추가
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iLevelNum)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iLevelNum)))
	{
		MSG_BOX("Failed to Create Object_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iLevelNum; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	Safe_Release(m_pGameInstance);
}