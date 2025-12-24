#include "Monster_Darkwraith.h"
#include "GameInstance.h"

#include "Darkwraith_Idle.h"
#include "Darkwraith_Walk.h"
#include "Darkwraith_Attack.h"
#include "Darkwraith_Damaged.h"

#include "Weapon_Darkwraith.h"

#include "Bounding_Sphere.h"
#include "Collider.h"

CMonster_Darkwraith::CMonster_Darkwraith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CMonster_Darkwraith::CMonster_Darkwraith(const CMonster_Darkwraith& Prototype)
	: CContainerObject{ Prototype }
{
}

const _float4x4* CMonster_Darkwraith::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

void CMonster_Darkwraith::Set_Animation(_uint iAnimationIndex, _bool isLoop)
{
	m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

HRESULT CMonster_Darkwraith::Initialize_Prototype()
{
	m_eLayer = LAYER::MONSTER;

	return S_OK;
}

HRESULT CMonster_Darkwraith::Initialize(void* pArg)
{
	lstrcpy(m_szName, TEXT("Darkwraith"));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pModelCom->Set_Animation(WALK, false);

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(20.f, 0.f, 5.f, 1.f));

	// 플레이어 정보 세팅
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Player(ENUM_TO_UINT(LEVELID::GAMEPLAY)));
	Safe_AddRef(m_pPlayer);

	m_bIsCollisionEnabled = true;

	return S_OK;
}

void CMonster_Darkwraith::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	m_vPlayerPos = m_pPlayer->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::POSITION);
	m_pStateMachine->Set_VectorData(TEXT("Player_Position"), m_vPlayerPos);

	m_fDistance = Compute_Distance(m_vPlayerPos);
}

void CMonster_Darkwraith::Update(_float fTimeDelta)
{
	// BlackBoard에 데이터 저장
	m_pStateMachine->Set_FloatData(TEXT("Darkwraith_Distance"), m_fDistance);
	m_pStateMachine->Set_BoolData(TEXT("Darkwraith_Targeting"), m_fDistance <= 6.f);
	
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
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	// 콜라이더 업데이트
	m_pColliderBody->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CMonster_Darkwraith::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CMonster_Darkwraith::Render()
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

void CMonster_Darkwraith::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Weapon"))
	{
		Set_Damaged(true);
	}
}

void CMonster_Darkwraith::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Weapon"))
	{
		Set_Damaged(false);
	}
}

HRESULT CMonster_Darkwraith::Ready_Components()
{
	// For Com_Model
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Darkwraith"),
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
	SphereDesc.fRadius = 0.4f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius + 0.8f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderBody), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Darkwraith::Ready_States()
{
	if (FAILED(m_pStateMachine->Add_State(IDLE, CDarkwraith_Idle::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(WALK, CDarkwraith_Walk::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(ATTACK, CDarkwraith_Attack::Create(this))))
		return E_FAIL;

	if (FAILED(m_pStateMachine->Add_State(DAMAGED, CDarkwraith_Damaged::Create(this))))
		return E_FAIL;

	m_pStateMachine->Set_State(IDLE);

	return S_OK;
}

HRESULT CMonster_Darkwraith::Ready_PartObjects()
{
	// Weapon
	CWeapon_Darkwraith::WEAPON_DARKWRAITH_DESC WeaponDesc{};
	WeaponDesc.pSocketMatrix = Get_SocketMatrix("R_Weapon");
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Darkwraith"),
		TEXT("Part_Weapon_Darkwraith"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Darkwraith::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CMonster_Darkwraith* CMonster_Darkwraith::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Darkwraith* pInstance = new CMonster_Darkwraith(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Darkwraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Darkwraith::Clone(void* pArg)
{
	CMonster_Darkwraith* pInstance = new CMonster_Darkwraith(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Darkwraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Darkwraith::Free()
{
	__super::Free();

	Safe_Release(m_pColliderBody);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayer);
}
