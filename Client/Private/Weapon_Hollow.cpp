#include "Weapon_Hollow.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Player.h"

CWeapon_Hollow::CWeapon_Hollow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject { pDevice, pContext }
{
}

CWeapon_Hollow::CWeapon_Hollow(const CWeapon_Hollow& Prototype)
	: CPartObject { Prototype }
{
}

HRESULT CWeapon_Hollow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Hollow::Initialize(void* pArg)
{
	WEAPON_HOLLOW_DESC* pDesc = static_cast<WEAPON_HOLLOW_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eLayer = LAYER::WEAPON;
	m_bIsCollisionEnabled = false;

	return S_OK;
}

void CWeapon_Hollow::Update_Priority(_float fTimeDelta)
{
}

void CWeapon_Hollow::Update(_float fTimeDelta)
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

void CWeapon_Hollow::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CWeapon_Hollow::Render()
{
	m_bIsCollisionEnabled = true;
#ifdef _DEBUG
	if (m_bIsCollisionEnabled == true)
	{
		for (auto& pCollider : m_vecColliders)
			pCollider->Render();
	}
#endif // _DEBUG

	return S_OK;
}

void CWeapon_Hollow::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

void CWeapon_Hollow::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

HRESULT CWeapon_Hollow::Ready_Components()
{
	/* For Com_Collider */
	for (_uint i = 0; i < m_iColliderCnt; i++)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
		SphereDesc.fRadius = 0.2f;
		_float fOffset = SphereDesc.fRadius - 0.8f + (0.25f * i);
		SphereDesc.vCenter = _float3(0.f, 0.f, fOffset);

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_TO_UINT(LEVELID::GAMEPLAY),
			TEXT("Prototype_Component_Collider_Sphere"), &SphereDesc));

		m_vecColliders.push_back(pCollider);
	}

	return S_OK;
}

CWeapon_Hollow* CWeapon_Hollow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Hollow* pInstance = new CWeapon_Hollow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Hollow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Hollow::Clone(void* pArg)
{
	CWeapon_Hollow* pInstance = new CWeapon_Hollow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Hollow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Hollow::Free()
{
	__super::Free();

	for (auto& pCollider : m_vecColliders)
		Safe_Release(pCollider);
	m_vecColliders.clear();
}