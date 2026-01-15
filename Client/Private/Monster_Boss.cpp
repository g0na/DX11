#include "Monster_Boss.h"
#include "GameInstance.h"
#include "Camera_Free.h"

#include "Boss_Idle.h"
#include "Boss_Walk.h"
#include "Boss_Backstep.h"
#include "Boss_Attack.h"
#include "Boss_DashAttack.h"
#include "Boss_FlyAttack.h"
#include "Boss_Death.h"

#include "Weapon_Boss.h"
#include "FlyCollider_Boss.h"

#include "Bounding_Sphere.h"
#include "Collider.h"

#include "Blood_Boss.h"
#include "Dust.h"

CMonster_Boss::CMonster_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CMonster_Boss::CMonster_Boss(const CMonster_Boss& Prototype)
	: CContainerObject{ Prototype }
{
}

const _float4x4* CMonster_Boss::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

void CMonster_Boss::Set_Animation(_uint iAnimationIndex, _bool isLoop)
{
	m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

void CMonster_Boss::Set_Damaged(_uint iDamage)
{
	m_fCurHp -= iDamage;

	if (m_fCurHp <= 0)
	{
		m_bIsDead = true;
		m_pStateMachine->Set_BoolData(TEXT("Boss_Dead"), true);
		return;
	}
}

HRESULT CMonster_Boss::Initialize_Prototype()
{
	m_eLayer = LAYER::MONSTER;

	return S_OK;
}

HRESULT CMonster_Boss::Initialize(void* pArg)
{
	lstrcpy(m_szName, TEXT("Boss"));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pModelCom->Set_Animation(WALK, false);

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(3.03f, -2.19f, -27.54f, 1.f));
	// x: -13.734574, z : -8.797766
	//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-13.734574f, -15.24f, -8.797766f, 1.f));

	// 플레이어 정보 세팅
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Player(ENUM_TO_UINT(LEVELID::GAMEPLAY)));
	Safe_AddRef(m_pPlayer);

	// 카메라 정보 세팅
	m_pCamera_Free = static_cast<CCamera_Free*>(m_pPlayer->Find_PartObject(TEXT("Part_Camera")));
	Safe_AddRef(m_pCamera_Free);

	if (m_pPlayer == nullptr ||
		m_pCamera_Free == nullptr)
		return E_FAIL;

	m_bIsCollisionEnabled = true;

	// 체력 설정
	m_fMaxHp = 1000;
	m_fCurHp = m_fMaxHp;

	return S_OK;
}

void CMonster_Boss::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	m_vPlayerPos = m_pPlayer->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::POSITION);
	m_pStateMachine->Set_VectorData(TEXT("Player_Position"), m_vPlayerPos);

	m_fDistance = Compute_Distance(m_vPlayerPos);
	m_fHeightDistance = Compute_Height(m_vPlayerPos);
}

void CMonster_Boss::Update(_float fTimeDelta)
{
	// UI ON/OFF
	if (m_fDistance <= 20.f && m_fHeightDistance <= 4.f)
	{
		m_pGameInstance->Show_UI(TEXT("Prototype_UI_Boss_HP"));
		m_pGameInstance->Show_UI(TEXT("Prototype_UI_Boss_Gauge"));
	}
	else
	{
		m_pGameInstance->Hide_UI(TEXT("Prototype_UI_Boss_HP"));
		m_pGameInstance->Hide_UI(TEXT("Prototype_UI_Boss_Gauge"));
	}

	if (m_pStateMachine->Get_BoolData(TEXT("Boss_Dead"), false) == true)
	{
		m_pGameInstance->Hide_UI(TEXT("Prototype_UI_Boss_HP"));
		m_pGameInstance->Hide_UI(TEXT("Prototype_UI_Boss_Gauge"));
	}

	// Dust 이펙트
	if (m_pStateMachine->Get_BoolData(TEXT("Dust_Enable"), false) == true)
	{
		Play_Dust(m_vPlayerPos);
		m_pStateMachine->Set_BoolData(TEXT("Dust_Enable"), false);
	}

	// 카메라 쉐이크
	if (m_pStateMachine->Get_BoolData(TEXT("Shake_Enable"), false) == true)
	{
		Add_Shake(0.8f);
		m_pStateMachine->Set_BoolData(TEXT("Shake_Enable"), false);
	}

	// BlackBoard에 데이터 저장
	m_pStateMachine->Set_FloatData(TEXT("Boss_Distance"), m_fDistance);
	m_pStateMachine->Set_FloatData(TEXT("Boss_Height"), m_fHeightDistance);
	m_pStateMachine->Set_BoolData(TEXT("Boss_Targeting"), m_fDistance <= 10.f && m_fHeightDistance <= 6.f);
	
	// 상태머신 업데이트
	m_pStateMachine->Update_State(fTimeDelta);

	// PartObjects 업데이트
	__super::Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	_vector vRootMotionDelta = m_pModelCom->Get_RootMotionDelta();

	_float3	fDelta;
	XMStoreFloat3(&fDelta, vRootMotionDelta);

	// 로컬 공간의 루트 모션 델타를 월드 공간의 벡터로 바꿔줘야 내가 바꾼 회전이 적용댐!
	_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
	_vector vUp = m_pTransformCom->Get_State(STATE::UP);
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	_vector vWorldDelta = vRight * fDelta.x + vUp * fDelta.y + vLook * fDelta.z;
	
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	vPosition += vWorldDelta;
	vPosition = m_pNavigationCom->Move(m_pTransformCom->Get_State(STATE::POSITION), vPosition);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	// 콜라이더 업데이트
	m_pColliderBody->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	// 디버깅
	_char buf[128];
	sprintf_s(buf, "Height : %f\n", m_fHeightDistance);
	OutputDebugStringA(buf);
}

void CMonster_Boss::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	if (m_fHeightDistance > 4.f)
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(3.03f, -2.19f, -27.54f, 1.f));

	// 체력, 스태미나 비율 계산
	m_fHpRatio = m_fCurHp / m_fMaxHp;

	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CMonster_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderBody->Render();
#endif

	return S_OK;
}

void CMonster_Boss::Play_Dust(_fvector vPosition)
{
	// 이펙트 리스트 가져오기
	list<CGameObject*> pListEffects = m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Effect"));

	// 이펙트 리스트 순회하면서 Dust만 따로 리스트에 삽입하기
	list<CDust*> pListDusts;
	for (auto& pEffect : pListEffects)
	{
		if (!wcscmp(TEXT("Effect_Dust"), pEffect->Get_Name()))
			pListDusts.push_back(static_cast<CDust*>(pEffect));
	}

	// Dust 리스트 순회하면서 비활성화 된 거 찾으면 Play
	for (auto& pDust : pListDusts)
	{
		if (!pDust->Get_IsOn())
		{
			pDust->Play(vPosition);
			return;
		}
	}
}

void CMonster_Boss::Play_Blood(_fvector vPosition)
{
	// 이펙트 리스트 가져오기
	list<CGameObject*> pListEffects = m_pGameInstance->Get_ObjectList(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Effect"));

	// 이펙트 리스트 순회하면서 Blood만 따로 리스트에 삽입하기
	list<CBlood_Boss*> pListBloods;
	for (auto& pEffect : pListEffects)
	{
		if (!wcscmp(TEXT("Effect_Blood_Boss"), pEffect->Get_Name()))
			pListBloods.push_back(static_cast<CBlood_Boss*>(pEffect));
	}

	// Dust 리스트 순회하면서 비활성화 된 거 찾으면 Play
	for (auto& pBlood : pListBloods)
	{
		if (!pBlood->Get_IsOn())
		{
			pBlood->Play(vPosition, m_pPlayer->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::POSITION));
			return;
		}
	}
}

void CMonster_Boss::Add_Shake(_float fAmount)
{
	m_pCamera_Free->Add_Shake(fAmount);
}

void CMonster_Boss::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Weapon"))
	{
		// 사운드 재생
		if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
			m_pGameInstance->PlaySoundW(TEXT("Boss_damage.wav"), CHANNELID::SOUND_PARTICLE, 1.f);
		else
			m_pGameInstance->PlaySoundW(TEXT("Boss_damage2.wav"), CHANNELID::SOUND_PARTICLE, 1.f);

		// 출혈 이펙트
		_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
		_vector vDir = XMVector3Normalize(m_pPlayer->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::POSITION) - vPosition);

		_float fY = XMVectorGetY(vPosition);
		fY += 1.f;
		vPosition = XMVectorSetY(vPosition, fY);
		vPosition = vPosition + (vDir * 2.f);
		Play_Blood(vPosition);

		Set_Damaged(250);
	}
}

void CMonster_Boss::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Weapon"))
	{
	}
}

HRESULT CMonster_Boss::Ready_Components()
{
	// For Com_Model
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Boss"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// For Com_Shader
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// For Com_StateMachine
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), reinterpret_cast<CComponent**>(&m_pStateMachine))))
		return E_FAIL;

	// For Com_Collider
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
	SphereDesc.fRadius = 3.5f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius + 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderBody), &SphereDesc)))
		return E_FAIL;

	// For Com_Navigation
	CNavigation::NAVIGATION_DESC NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = 768;
	NavigationDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boss::Ready_States()
{
	if (FAILED(m_pStateMachine->Add_State(IDLE, CBoss_Idle::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(WALK, CBoss_Walk::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(BACKSTEP, CBoss_Backstep::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(ATTACK, CBoss_Attack::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(DASHATTACK, CBoss_DashAttack::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(FLYATTACK, CBoss_FlyAttack::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(DEATH, CBoss_Death::Create(this))))
		return E_FAIL;

	m_pStateMachine->Set_State(IDLE);

	return S_OK;
}

HRESULT CMonster_Boss::Ready_PartObjects()
{
	// Weapon
	CWeapon_Boss::WEAPON_BOSS_DESC WeaponDesc{};
	WeaponDesc.pSocketMatrix = Get_SocketMatrix("R_weapon");
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Boss"),
		TEXT("Part_Weapon_Boss"), &WeaponDesc)))
		return E_FAIL;

	// Fly Collider
	CFlyCollider_Boss::FLYCOLLIDER_BOSS_DESC FlyColliderDesc{};
	FlyColliderDesc.pSocketMatrix = Get_SocketMatrix("c2230");
	FlyColliderDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_FlyCollider_Boss"),
		TEXT("Part_FlyCollider_Boss"), &FlyColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boss::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CMonster_Boss* CMonster_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Boss* pInstance = new CMonster_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Boss::Clone(void* pArg)
{
	CMonster_Boss* pInstance = new CMonster_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderBody);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pCamera_Free);
	Safe_Release(m_pPlayer);
}
