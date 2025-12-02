#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "ImGui_Manager.h"

USING(Maptool)

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

	if (FAILED(Ready_Layer_Background(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임 플레이 레벨입니다."));
#endif

	CImGui_Manager::GetInstance()->Render();

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
	lstrcpy(CameraDesc.szName, TEXT("Main_Camera"));
	CameraDesc.vPosition = _float3(0.f, 30.f, -20.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 25.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.0f);
	CameraDesc.fFovY = XMConvertToRadians(45.0f);
	CameraDesc.fNearZ = 0.1f;
	CameraDesc.fFarZ = 1000.f;
	CameraDesc.fSensor = 0.1f;

	if (m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag, &CameraDesc) == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	//CGameObject::GAMEOBJECT_DESC	GameObjectDesc{};
	//lstrcpy(GameObjectDesc.szName, TEXT("Darkwraith"));

	//if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
	//	ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag, &GameObjectDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Background(const _wstring& strLayerTag)
{
	//CGameObject::GAMEOBJECT_DESC	GameObjectDesc{};
	//lstrcpy(GameObjectDesc.szName, TEXT("Map"));

	//if (m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Map"),
	//	ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag, &GameObjectDesc) == nullptr)
	//	return E_FAIL;

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