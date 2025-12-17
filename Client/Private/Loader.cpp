#include "Loader.h"	
#include "Background.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Camera_Free.h"

#include "Monster_Darkwraith.h"
#include "Weapon_Darkwraith.h"

#include "Monster_Boss.h"
#include "Weapon_Boss.h"

#include "Monster_Hollow.h"

#include "Map.h"
#include "Navigation.h"

#include "Player.h"
#include "Body.h"
#include "Weapon.h"
#include "WeaponCase.h"
#include "Shield.h"

#include "Collider.h"
#include "StateMachine.h"

USING(Client)

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
	, m_pGameInstance {CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

// 보조 스레드의 작업 시작점
//_uint APIENTRY ThreadMain(void* pArg)
//{
//	// 전달받은 this를 받는다
//	CLoader* pLoader = static_cast<CLoader*>(pArg);
//
//	// 전달받은 매개 변수에 따라서 로딩을 시작한다.
//	if (FAILED(pLoader->Loading()))
//		return 1;
//
//	return 0;
//}

HRESULT CLoader::Initialize(LEVELID eLoadingLevelID)
{
	m_eLoadingLevelID = eLoadingLevelID;

	m_Thread = thread(&CLoader::Loading, this);

	// 작업 공간(임계 영역)을 준비
	//InitializeCriticalSection(&m_CriticalSection);

	// 보조 스레드를 생성하는 함수
	//m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	//if (m_hThread == 0)
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{ 
	// 보조 스레드가 작업 공간을 사용한다고 선언 (잠금)
	//EnterCriticalSection(&m_CriticalSection);
	
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

	// 작업 공간 사용이 끝났음을 선언 (잠금 해제)
	//LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Logo()
{
	UpdateLoadingText(TEXT("텍스쳐를 로딩 중 입니다."));

	// For Prototype_Component_Texture_Background
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::LOGO), TEXT("Prototype_Component_Texture_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default0.jpg"), 1))))
		return E_FAIL;
	
	UpdateLoadingText(TEXT("모델을(를) 로딩 중 입니다."));

	UpdateLoadingText(TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));

	UpdateLoadingText(TEXT("객체원형을(를) 로딩 중 입니다."));
	// For Prototype_GameObject_Backgroud
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::LOGO), TEXT("Prototype_GameObject_Background"),
		CBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	UpdateLoadingText(TEXT("로딩이 완료되었습니다."));

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_GamePlay()
{
	UpdateLoadingText(TEXT("컴포넌트를 로딩 중 입니다."));
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;	

	/* For.Prototype_Component_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE))))
		return E_FAIL;
	
	UpdateLoadingText(TEXT("모델을(를) 로딩 중 입니다."));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	// 모델에 필요한 초기 상태 행렬 선언
	_matrix PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	/* For.Prototype_Component_Model_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Boss/Boss.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Darkwraith */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Darkwraith"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Darkwraith/Darkwraith.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Hollow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Hollow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Hollow/Hollow.fbx", PreTransformMatrix))))
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

	UpdateLoadingText(TEXT("셰이더을(를) 로딩 중 입니다."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	UpdateLoadingText(TEXT("네비게이션을(를) 로딩 중 입니다."));
	vector<const _tchar*> navigationDatas;
	navigationDatas.reserve(19);
#pragma region NAVIGATION_DATAS
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0000B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0001B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0003B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0004B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0005B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0006B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0007B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0008B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0009B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0010B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0011B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0013B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0014B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0015B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0016B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0050B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0052B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0053B1A18.nvm"));
	navigationDatas.push_back(TEXT("../Bin/DataFiles/n0054B1A18.nvm"));
#pragma endregion

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, navigationDatas))))
		return E_FAIL;

	UpdateLoadingText(TEXT("객체원형을(를) 로딩 중 입니다."));
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Darkwraith */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Darkwraith"),
		CMonster_Darkwraith::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Darkwraith */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Darkwraith"),
		CWeapon_Darkwraith::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Boss"),
		CMonster_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Boss"),
		CWeapon_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hollow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Hollow"),
		CMonster_Hollow::Create(m_pDevice, m_pContext))))
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

	// 보조 스레드의 작업이 완전히 마칠 때까지 기다린다.
	//WaitForSingleObject(m_hThread, INFINITE);

	// 사용했던 작업 공간 (임계 영역)을 정리한다.
	//DeleteCriticalSection(&m_CriticalSection);

	// 보조 스레드의 핸들을 정리한다.
	//CloseHandle(m_hThread);

	if (m_Thread.joinable())
		m_Thread.join();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}