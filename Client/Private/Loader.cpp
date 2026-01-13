#include "Loader.h"	
#include "GameInstance.h"
#include "Camera_Free.h"

#include "Background.h"
#include "Player_Slot.h"
#include "UI_Estus.h"
#include "UI_Weapon.h"
#include "UI_Shield.h"
#include "UI_Player_Gauge.h"
#include "UI_Player_HP.h"
#include "UI_Player_Stamina_Gauge.h"
#include "UI_Player_Stamina.h"
#include "UI_Boss_Gauge.h"
#include "UI_Boss_HP.h"
#include "UI_PopUp.h"
#include "UI_PopUp_Door.h"
#include "UI_PopUp_PickUp.h"
#include "UI_PopUp_Estus.h"
#include "UI_PopUp_Weapon.h"
#include "UI_PopUp_Shield.h"
#include "UI_Victory.h"
#include "UI_Victory_Back.h"

#include "Monster_Darkwraith.h"
#include "Weapon_Darkwraith.h"

#include "Monster_Boss.h"
#include "Weapon_Boss.h"
#include "FlyCollider_Boss.h"

#include "Monster_Hollow.h"
#include "Weapon_Hollow.h"

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
#include "Item.h"
#include "Item_Weapon.h"
#include "Item_Shield.h"
#include "Fire.h"
#include "Blood.h"
#include "Blood_Boss.h"

#include "Navigation.h"

#include "Player.h"
#include "Body.h"
#include "Weapon.h"
#include "WeaponCase.h"
#include "Shield.h"
#include "Estus.h"

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/menu_local-tpf/Title.dds"), 1))))
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
	UpdateLoadingText(TEXT("텍스쳐를 로딩 중 입니다."));
	// For Prototype_Component_Texture_PlayerSlot
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_PlayerSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Player_SlotBG1.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Estus
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Estus"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Estus%d.dds"), 2))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Weapon
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Weapon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Weapon.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Shield
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Shield"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Shield.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Player_Gauge
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Player_Gauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Player_Gauge.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Player_HP
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Player_HP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Player_HP.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Player_Stamina
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Player_Stamina"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Player_Stamina.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Boss_Gauge
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Boss_Gauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Boss_Gauge.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_PopUp
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_PopUp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/PopUp_Slim.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_PopUp_Estus
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_PopUp_Estus"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/PopUp_Estus.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_PopUp_Weapon
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_PopUp_Weapon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/PopUp_Weapon.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_PopUp_Shield
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_PopUp_Shield"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/PopUp_Shield.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Victory
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Victory"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Victory.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_UI_Victory_Back
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Victory_Back"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/bk-tpf/Victory_Back.dds"), 1))))
		return E_FAIL;

	// For Prototype_Component_Texture_Fire
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Fire/fire%d.dds"), 3))))
		return E_FAIL;

	// For Prototype_Component_Texture_Blood
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/blood/blood.dds"), 1))))
		return E_FAIL;

	UpdateLoadingText(TEXT("컴포넌트를 로딩 중 입니다."));
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

	/* For.Prototype_Component_VIBuffer_Particle_Blood */
	CVIBuffer_Particle_Point::PARTICLE_POINT_DESC	BloodDesc{};
	BloodDesc.iNumInstance = 250;
	BloodDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BloodDesc.vSize = _float2(0.015f, 0.02f);
	BloodDesc.vRange = _float3(0.1f, 0.05f, 0.1f);
	BloodDesc.vSpeed = _float2(2.5f, 4.f);
	BloodDesc.vLifeTime = _float2(1.f, 1.5f);
	BloodDesc.isLoop = false;
	BloodDesc.vPivot = _float3(0.f, -0.5f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Blood"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &BloodDesc))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Blood_Boss */
	CVIBuffer_Particle_Point::PARTICLE_POINT_DESC	BloodBossDesc{};
	BloodBossDesc.iNumInstance = 500;
	BloodBossDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BloodBossDesc.vSize = _float2(0.015f, 0.03f);
	BloodBossDesc.vRange = _float3(0.3f, 0.15f, 0.3f);
	BloodBossDesc.vSpeed = _float2(2.5f, 5.f);
	BloodBossDesc.vLifeTime = _float2(1.f, 1.5f);
	BloodBossDesc.isLoop = false;
	BloodBossDesc.vPivot = _float3(0.f, -0.5f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Blood_Boss"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &BloodBossDesc))))
		return E_FAIL;
	
	UpdateLoadingText(TEXT("모델을(를) 로딩 중 입니다."));

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
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Player_Final.fbx", PreTransformMatrix))))
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

	PreTransformMatrix *= XMMatrixScaling(0.45f, 0.45f, 0.45f);
	/* For.Prototype_Component_Model_Estus */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Estus"), 
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Estus_Flask/Estus_Flask.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixIdentity();
	//PreTransformMatrix *= XMMatrixRotationY(XMConvertToRadians(-180.f));
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

	/* For.Prototype_Component_Model_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
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

	/* For.Prototype_Component_Shader_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_Fire"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Fire.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPos_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxPos_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPos_Particle.hlsl"), VTXPOS_PARTICLE::Elements, VTXPOS_PARTICLE::iNumElements))))
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
	/* For.Prototype_GameObject_PlayerSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_PlayerSlot"),
		CPlayer_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Estus */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Estus"),
		CUI_Estus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Weapon"),
		CUI_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Shield"),
		CUI_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Player_Gauge */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_Gauge"),
		CUI_Player_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Player_HP */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_HP"),
		CUI_Player_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Player_Stamina_Gauge */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_Stamina_Gauge"),
		CUI_Player_Stamina_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Player_Stamina */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Player_Stamina"),
		CUI_Player_Stamina::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Boss_Gauge */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Boss_Gauge"),
		CUI_Boss_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Boss_HP */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Boss_HP"),
		CUI_Boss_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PopUp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp"),
		CUI_PopUp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PopUp_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Door"),
		CUI_PopUp_Door::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PopUp_PickUp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_PickUp"),
		CUI_PopUp_PickUp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PopUp_Estus */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Estus"),
		CUI_PopUp_Estus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PopUp_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Weapon"),
		CUI_PopUp_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_PopUp_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_PopUp_Shield"),
		CUI_PopUp_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Victory */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Victory"),
		CUI_Victory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Victory_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_UI_Victory_Back"),
		CUI_Victory_Back::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_FlyCollider_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_FlyCollider_Boss"),
		CFlyCollider_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hollow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Hollow"),
		CMonster_Hollow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Hollow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Hollow"),
		CWeapon_Hollow::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Estus */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Estus"),
		CEstus::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Item_Estus */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Item_Estus"),
		CItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Item_Weapon"),
		CItem_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Item_Shield"),
		CItem_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Fire"),
		CFire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Blood */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Blood"),
		CBlood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Blood_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Blood_Boss"),
		CBlood_Boss::Create(m_pDevice, m_pContext))))
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