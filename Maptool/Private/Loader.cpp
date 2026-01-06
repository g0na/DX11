#include "Loader.h"	
#include "GameInstance.h"
#include "Camera_Free.h"

#include "Monster_Darkwraith.h"
#include "Monster_Hollow.h"

#include "Sky.h"
#include "Fog.h"
#include "Mountain.h"
#include "Map.h"
#include "Map2.h"
#include "Map3.h"
#include "Map4.h"
#include "Ladder.h"
#include "InDoor.h"
#include "OutDoor.h"

#include "Player.h"
#include "Body.h"
#include "Weapon.h"
#include "WeaponCase.h"
#include "Shield.h"

#include "Calculator.h"
#include "StateMachine.h"

USING(Maptool)

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
	, m_pGameInstance {CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLoader::Initialize(LEVELID eLoadingLevelID)
{
	m_eLoadingLevelID = eLoadingLevelID;

	m_Thread = thread(&CLoader::Loading, this);

	return S_OK;
}

HRESULT CLoader::Loading()
{ 
	// COM 라이브러리 초기화
	CoInitializeEx(nullptr, 0);

	HRESULT hr = {};

	switch (m_eLoadingLevelID)
	{
	case LEVELID::LOGO:
		hr = Loading_Logo();

		break;

	case LEVELID::GAMEPLAY:
		hr = Loading_GamePlay();

		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Logo()
{
	UpdateLoadingText(TEXT("텍스쳐를 로딩 중 입니다."));
	
	UpdateLoadingText(TEXT("모델을(를) 로딩 중 입니다."));

	UpdateLoadingText(TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));

	UpdateLoadingText(TEXT("객체원형을(를) 로딩 중 입니다."));	

	UpdateLoadingText(TEXT("로딩이 완료되었습니다."));

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_GamePlay()
{
	UpdateLoadingText(TEXT("텍스쳐를 로딩 중 입니다."));
	
	UpdateLoadingText(TEXT("모델을(를) 로딩 중 입니다."));
	_matrix PreTransformMatrix = DirectX::XMMatrixIdentity();
	PreTransformMatrix *= XMMatrixRotationY(XMConvertToRadians(180.f));

	/* For.Prototype_Component_Model_Darkwraith */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Darkwraith"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Darkwraith/Darkwraith.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Hollow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Hollow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Hollow/Hollow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Player50.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Player_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Player_Weapon"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Player_Sword/Player_Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Player_Weaponcase */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Player_Weaponcase"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Player_Sword/Player_Swordcase.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Player_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Player_Shield"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Player_Shield/Player_Shield.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/Sky.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fog */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Fog"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/Fog.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mountain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Mountain"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/Mountain.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Map1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Map1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/map_part1.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Map2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Map2"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/map_part2.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Map3 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Map3"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/map_part3.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Map4 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Map4"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/map_part4.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ladder */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Ladder"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Maps/ladder.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_InDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_InDoor"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Maps/InDoor.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_OutDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_OutDoor"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Maps/OutDoor.fbx", PreTransformMatrix))))
		return E_FAIL;
	
	/* For.Prototype_Component_Calculator */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Calculator"),
		CCalculator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	UpdateLoadingText(TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));
	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	UpdateLoadingText(TEXT("객체원형을(를) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Darkwraith */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Darkwraith"),
		CMonster_Darkwraith::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hollow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Hollow"),
		CMonster_Hollow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Fog */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Fog"),
		CFog::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Mountain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Mountain"),
		CMountain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Map1"),
		CMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map2 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Map2"),
		CMap2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map3 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Map3"),
		CMap3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map4 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Map4"),
		CMap4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ladder */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Ladder"),
		CLadder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_InDoor"),
		CInDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_OutDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_OutDoor"),
		COutDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		CBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Player"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WeaponCase_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_WeaponCase_Player"),
		CWeaponCase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Shield_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Shield_Player"),
		CShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	UpdateLoadingText(TEXT("로딩이 완료되었슴니다."));

	m_bIsFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(LEVELID eNextLevelID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Create CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	if (m_Thread.joinable())
		m_Thread.join();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}