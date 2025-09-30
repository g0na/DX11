#include "Prototype_Manager.h"
#include "GameObject.h"

USING(Engine)

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT	CPrototype_Manager::Initialize(_uint iLevelNum)
{
	m_iLevelNum = iLevelNum;

	m_pPrototypes = new map<const _wstring, CBase*>[iLevelNum];
	
	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelNum, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	// 원본을 추가할 때 중복 키가 허용되지 않으므로 nullptr이 아니거나 (= 이미 원본이 만들어져 있다.)
	// 동적 생성한 최대 개수보다 크면 실패
	if (iLevelNum >= m_iLevelNum ||
		Find_Prototype(iLevelNum, strPrototypeTag) != nullptr)
		return E_FAIL;

	m_pPrototypes[iLevelNum].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelNum, const _wstring& strPrototypeTag, void* pArg)
{
	// 특정 레벨의 원본을 먼저 검색
	CBase* pPrototype = Find_Prototype(iLevelNum, strPrototypeTag);
	if (pPrototype == nullptr)
		return nullptr;

	// 원본이 있다면 올바른 객체를 Clone
	// CBase* 인 pPrototype에는 Clone 함수가 없으므로 임의로 열거체를 만들어서 조건을 건다.
	// 조건에 맞는 형변환을 하여 Clone 함수를 호출
	return ePrototypeID == PROTOTYPE::GAMEOBJECT ?
		dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg) :
		/*dynamic_cast<CComponent*>(pPrototype)->Clone(pArg)*/ nullptr;
}

void CPrototype_Manager::Clear(_uint iLevelNum)
{
	// 특정 레벨의 프로토타입을 순회한다.
	for (auto& Pair : m_pPrototypes[iLevelNum])
		// 프로토타입을 삭제한다 
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelNum].clear();
}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelNum, const _wstring& strProtoTag)
{
	auto iter = m_pPrototypes[iLevelNum].find(strProtoTag);

	if (iter == m_pPrototypes[iLevelNum].end())
		return nullptr;
	
	return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iLevelNum)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize(iLevelNum)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iLevelNum; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
