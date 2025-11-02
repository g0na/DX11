#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel {pDevice, pContext}
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	// 다음 레벨에 대한 자원을 준비한다.
	m_pLoader = CLoader::Create(m_eNextLevelID, m_pDevice, m_pContext);
	if (m_pLoader == nullptr)
		return E_FAIL;

	// 자원을 준비하는 동안 로딩 장면을 렌더링 한다. (메인 스레드)
	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (m_pLoader->isFinished() == true &&
		GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVELID::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;

		case LEVELID::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		}

		if (pNewLevel == nullptr)
			return;

		if (SUCCEEDED(m_pGameInstance->Change_Level(ENUM_TO_UINT(m_eNextLevelID), pNewLevel)))
			return;

		MSG_BOX("오류 발생");
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->OutPut();
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Create CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
