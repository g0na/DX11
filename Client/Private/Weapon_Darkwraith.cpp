#include "Weapon_Darkwraith.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Player.h"

CWeapon_Darkwraith::CWeapon_Darkwraith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject { pDevice, pContext }
{
}

CWeapon_Darkwraith::CWeapon_Darkwraith(const CWeapon_Darkwraith& Prototype)
	: CPartObject { Prototype }
{
}

HRESULT CWeapon_Darkwraith::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Darkwraith::Initialize(void* pArg)
{
	WEAPON_DARKWRAITH_DESC* pDesc = static_cast<WEAPON_DARKWRAITH_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eLayer = LAYER::WEAPON;
	m_bIsCollisionEnabled = false;

	return S_OK;
}

void CWeapon_Darkwraith::Update_Priority(_float fTimeDelta)
{
}

void CWeapon_Darkwraith::Update(_float fTimeDelta)
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

void CWeapon_Darkwraith::Update_Late(_float fTimeDelta)
{
#ifdef _DEBUG
	for (_uint i = 0; i < m_iColliderCnt; i++)
		m_pGameInstance->Add_DebugComponent(m_vecColliders[i]);
#endif // _DEBUG

	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CWeapon_Darkwraith::Render()
{
#ifdef _DEBUG
	if (m_bIsCollisionEnabled == true)
	{
		for (auto& pCollider : m_vecColliders)
			pCollider->Render();
	}
#endif // _DEBUG

	return S_OK;
}

void CWeapon_Darkwraith::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

void CWeapon_Darkwraith::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

HRESULT CWeapon_Darkwraith::Ready_Components()
{
	/* For Com_Collider */
	for (_uint i = 0; i < m_iColliderCnt; i++)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
		SphereDesc.fRadius = 0.1f;
		_float fOffset = SphereDesc.fRadius - 1.2f + (0.25f * i);
		SphereDesc.vCenter = _float3(0.f, 0.f, fOffset);

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_TO_UINT(LEVELID::GAMEPLAY),
			TEXT("Prototype_Component_Collider_Sphere"), &SphereDesc));

		m_vecColliders.push_back(pCollider);
	}

	return S_OK;
}

CWeapon_Darkwraith* CWeapon_Darkwraith::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Darkwraith* pInstance = new CWeapon_Darkwraith(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Darkwraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Darkwraith::Clone(void* pArg)
{
	CWeapon_Darkwraith* pInstance = new CWeapon_Darkwraith(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Darkwraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Darkwraith::Free()
{
	__super::Free();

	for (auto& pCollider : m_vecColliders)
		Safe_Release(pCollider);
	m_vecColliders.clear();
}