#include "Weapon_Boss.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Player.h"

CWeapon_Boss::CWeapon_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject { pDevice, pContext }
{
}

CWeapon_Boss::CWeapon_Boss(const CWeapon_Boss& Prototype)
	: CPartObject { Prototype }
{
}

HRESULT CWeapon_Boss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Boss::Initialize(void* pArg)
{
	WEAPON_BOSS_DESC* pDesc = static_cast<WEAPON_BOSS_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eLayer = LAYER::WEAPON;
	//m_bIsCollisionEnabled = true;

	return S_OK;
}

void CWeapon_Boss::Update_Priority(_float fTimeDelta)
{
}

void CWeapon_Boss::Update(_float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (_uint i = 0; i < 3; i++)
		// 스케일 값을 정규화한다.
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	_matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentMatrix);

	__super::SetUp_CombinedWorldMatrix(ParentMatrix);

	for (auto& pCollider : m_vecColliders)
		pCollider->Update(XMLoadFloat4x4(&m_CombinedWorldMartix));
}

void CWeapon_Boss::Update_Late(_float fTimeDelta)
{
#ifdef _DEBUG
	for (_uint i = 0; i < m_iColliderCnt; i++)
		m_pGameInstance->Add_DebugComponent(m_vecColliders[i]);
#endif // _DEBUG

	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CWeapon_Boss::Render()
{
	//m_bIsCollisionEnabled = true;
#ifdef _DEBUG
	if (m_bIsCollisionEnabled == true)
	{
		for (auto& pCollider : m_vecColliders)
			pCollider->Render();
	}
#endif // _DEBUG

	return S_OK;
}

void CWeapon_Boss::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

void CWeapon_Boss::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

HRESULT CWeapon_Boss::Ready_Components()
{
	/* For Com_Collider */
	for (_uint i = 0; i < m_iColliderCnt; i++)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
		SphereDesc.fRadius = 1.2f;
		_float fOffset = SphereDesc.fRadius - 1.4f - (1.4f * i);
		SphereDesc.vCenter = _float3(0.f, fOffset, 0.f);

		if (i == m_iColliderCnt - 1)
		{
			SphereDesc.fRadius = 1.4f;
			SphereDesc.vCenter = _float3(-0.9f, fOffset - 0.35f, 0.f);
		}

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_TO_UINT(LEVELID::GAMEPLAY),
			TEXT("Prototype_Component_Collider_Sphere"), &SphereDesc));

		m_vecColliders.push_back(pCollider);
	}

	return S_OK;
}

CWeapon_Boss* CWeapon_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Boss* pInstance = new CWeapon_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Boss::Clone(void* pArg)
{
	CWeapon_Boss* pInstance = new CWeapon_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Boss::Free()
{
	__super::Free();

	for (auto& pCollider : m_vecColliders)
		Safe_Release(pCollider);
	m_vecColliders.clear();
}