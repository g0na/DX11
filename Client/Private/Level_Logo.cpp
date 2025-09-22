#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_GamePlay.h"

USING(Client);

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel {pDevice, pContext}
{
}

HRESULT CLevel_Logo::Initialize()
{
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_TO_UINT(LEVELID::GAMEPLAY), CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
