#include "Weapon_Darkwraith.h"
#include "GameInstance.h"
#include "Collider.h"

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
	m_bIsCollisionEnabled = true;

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

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMartix));
}

void CWeapon_Darkwraith::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CWeapon_Darkwraith::Render()
{
	m_pColliderCom->Render();

	return S_OK;
}

void CWeapon_Darkwraith::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
		int a = 10;
}

void CWeapon_Darkwraith::OnCollisionExit(CGameObject* pOtherObject)
{
}

HRESULT CWeapon_Darkwraith::Ready_Components()
{
	// For Com_Collider
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
	SphereDesc.fRadius = 0.1f;
	SphereDesc.vCenter = _float3(0.f, 0.f, -1.f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;
		
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

	Safe_Release(m_pColliderCom);
}