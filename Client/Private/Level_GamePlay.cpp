#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Player.h"
#include "UIObj.h"

USING(Client)

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel {pDevice, pContext}
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_UIs(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	// Player, Object 충돌 체크
	m_pGameInstance->Check_Collision(m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Player")),
		m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Object")));

	// Player, Monster 충돌 체크
	m_pGameInstance->Check_Collision(m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Player")),
		m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Monster")));

	// Player_Weapon, Monster 충돌 체크
	m_pGameInstance->Check_Collision(static_cast<CPlayer*>(m_pGameInstance->Get_Player(ENUM_TO_UINT(LEVELID::GAMEPLAY)))->Get_PartObjects(),
		m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Monster")));

	// Player, Monster_Weapon 충돌 체크
	list<CGameObject*> MonsterList = m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Monster"));
	list<CGameObject*> MonsterPartList = {};
	for (auto& pMonster : MonsterList)
	{
		for (auto& Part : static_cast<CContainerObject*>(pMonster)->Get_PartObjects())
		{
			if (Part == nullptr)
				continue;

			MonsterPartList.push_back(Part);
		}
	}

	m_pGameInstance->Check_Collision(m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Player")), MonsterPartList);
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
	LightDesc.vDiffuse = { 0.8f, 0.8f, 0.8f, 1.f };
	LightDesc.vAmbient = { 0.3f, 0.3f, 0.3f, 1.f };
	LightDesc.vSpecular = { 0.15f, 0.15f, 0.15f, 1.f };

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Fonts()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Korean"), TEXT("../Bin/Resources/Fonts/Korean.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UIs(const _wstring& strLayerTag)
{
	// Player_Slot
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_PlayerSlot"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_PlayerSlot"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Estus
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Estus"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Estus"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Weapon
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Weapon"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Weapon"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Shield
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Shield"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Shield"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Player_HP
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Player_HP"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_HP"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Player_Gauge
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Player_Gauge"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_Gauge"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Player_Stamina
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Player_Stamina"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_Stamina"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Player_Stamina_Gauge
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Player_Stamina_Gauge"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_Stamina_Gauge"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Boss_HP
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Boss_HP"),
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Boss_HP"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Boss_Gauge
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Boss_Gauge"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Boss_Gauge"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_PopUp
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_PopUp"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_PopUp_Door
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_PopUp_Door"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Door"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_PopUp_PickUp
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_PopUp_PickUp"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_PickUp"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_PopUp_Estus
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_PopUp_Estus"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Estus"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_PopUp_Weapon
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_PopUp_Weapon"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Weapon"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_PopUp_Shield
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_PopUp_Shield"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Shield"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Victory_Back
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Victory_Back"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Victory_Back"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	// UI_Victory
	if (FAILED(m_pGameInstance->Add_UI(TEXT("Prototype_UI_Victory"),	
		static_cast<CUIObj*>(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Victory"),
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Boss"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const _wstring& strLayerTag)
{
	Load_Mapdata(TEXT("../Bin/DataFiles/Map_Objects.json"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Object(const _wstring& strLayerTag)
{
	if (m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Item_Estus"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag) == nullptr)
		return E_FAIL;

	if (m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Item_Weapon"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag) == nullptr)
		return E_FAIL;

	if (m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Item_Shield"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag) == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	if (m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Blood"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag) == nullptr)
		return E_FAIL;

	if (m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Blood_Boss"),
		ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag) == nullptr)
		return E_FAIL;

	return S_OK;
}

_wstring CLevel_GamePlay::CharToWstring(const _char* pString)
{
	_uint iStringSize = MultiByteToWideChar(CP_ACP, 0, pString, -1, NULL, 0);

	_wstring strResult(iStringSize - 1, 0);
	MultiByteToWideChar(CP_ACP, 0, pString, -1, &strResult[0], iStringSize);

	return strResult;
}

HRESULT CLevel_GamePlay::Load_Mapdata(const _tchar* pFilePath)
{
	ifstream fileJson(pFilePath, ios::in);
	if (fileJson.is_open() == false)
		return E_FAIL;

	m_strJsonPath = pFilePath;

	ordered_json objectDatas = ordered_json::array();
	fileJson >> objectDatas;
	fileJson.close();

	for (auto& objectData : objectDatas)
	{
		JSONGAMEOBJECT_DESC jsonDesc{};
		from_json(objectData, jsonDesc);

		_wstring strProtoTag = CharToWstring(jsonDesc.strPrototypeTag.c_str());
		_wstring strLayerTag = CharToWstring(jsonDesc.strLayerTag.c_str());
		m_vRotationAngle = jsonDesc.vRotation;

		CGameObject* pGameObject = m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVELID::GAMEPLAY), strProtoTag,
			ENUM_TO_UINT(LEVELID::GAMEPLAY), strLayerTag);
		if (pGameObject == nullptr)
			return E_FAIL;

		CTransform* pTransformCom = pGameObject->Get_Component<CTransform>(g_strTransformTag);
		pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&jsonDesc.vPosition), 1.f));
		pTransformCom->Set_Scale(jsonDesc.vScale.x, jsonDesc.vScale.y, jsonDesc.vScale.z);
		pTransformCom->Rotation(XMConvertToRadians(jsonDesc.vRotation.x), XMConvertToRadians(jsonDesc.vRotation.y), XMConvertToRadians(jsonDesc.vRotation.z));
		pTransformCom->Set_RotationAngle(m_vRotationAngle);
	}

	return S_OK;
}

void CLevel_GamePlay::from_json(const ordered_json& j, JSONGAMEOBJECT_DESC& jsonDesc)
{
	j["PrototypeTag"].get_to(jsonDesc.strPrototypeTag);
	j["LayerTag"].get_to(jsonDesc.strLayerTag);

	j["Position"][0].get_to(jsonDesc.vPosition.x);
	j["Position"][1].get_to(jsonDesc.vPosition.y);
	j["Position"][2].get_to(jsonDesc.vPosition.z);

	j["Rotation"][0].get_to(jsonDesc.vRotation.x);
	j["Rotation"][1].get_to(jsonDesc.vRotation.y);
	j["Rotation"][2].get_to(jsonDesc.vRotation.z);

	j["Scale"][0].get_to(jsonDesc.vScale.x);
	j["Scale"][1].get_to(jsonDesc.vScale.y);
	j["Scale"][2].get_to(jsonDesc.vScale.z);
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