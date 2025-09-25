#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

USING(Engine)

CLevel_Manager::CLevel_Manager() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Change_Level(_uint iCurLevelID, CLevel* pNewLevel)
{
	// 이전 레벨의 자원을 정리
	// 만약 레벨이 처음 생성되었다면 현재 레벨은 nullptr 일 것이다. 따라서 처음 생성되지 않았을 때에만 정리를 한다.
	if (m_pCurLevel != nullptr)
		m_pGameInstance->Clear(m_iCurLevelID);

	// 이전 레벨을 삭제
	Safe_Release(m_pCurLevel);

	// 변경할 레벨의 인덱스와 레벨 객체를 넣어준다.
	m_pCurLevel = pNewLevel;
	m_iCurLevelID = iCurLevelID;

	return S_OK;
}

void CLevel_Manager::Update_Level(_float fTimeDelta)
{
	m_pCurLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render_Level()
{
	m_pCurLevel->Render();

	return S_OK;
}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pCurLevel);
	Safe_Release(m_pGameInstance);
}