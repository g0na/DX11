#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Player.h"

USING(Client)

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel {pDevice, pContext}
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Background(TEXT("Layer_Background"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	// Player, Monster 충돌 체크
	m_pGameInstance->Check_Collision(m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Player")),
		m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Monster")));

	// Player_Weapon, Monster 충돌 체크
	m_pGameInstance->Check_Collision(dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(ENUM_TO_UINT(LEVELID::GAMEPLAY)))->Get_PartObjects(),
		m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Monster")));
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	//SetWindowText(g_hWnd, TEXT("게임 플레이 레벨입니다."));
	wchar_t szBuffer[255];
	swprintf_s(szBuffer, 255, L"FPS: %d, DT: %.8f", m_pGameInstance->Get_FPS(TEXT("Timer_60")), m_pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
	SetWindowText(g_hWnd, szBuffer);
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDirection = { 1.f, -1.f, 1.f, 0.f };
	LightDesc.vDiffuse = { 1.f, 1.f, 1.f, 1.f };
	LightDesc.vAmbient = { 1.f, 1.f, 1.f, 1.f };
	LightDesc.vSpecular = { 1.f, 1.f, 1.f, 1.f };

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};
	CameraDesc.vPosition = _float3(0.f, 30.f, -20.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 25.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.0f);
	CameraDesc.fFovY = XMConvertToRadians(45.0f);
	CameraDesc.fNearZ = 0.1f;
	CameraDesc.fFarZ = 1000.f;
	CameraDesc.fSensor = 0.1f;

	m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag, &CameraDesc);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Darkwraith"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag);

	m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Hollow"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Background(const _wstring& strLayerTag)
{
	m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag);

	return S_OK;
}


CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}