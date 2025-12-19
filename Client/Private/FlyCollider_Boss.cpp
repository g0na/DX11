#include "FlyCollider_Boss.h"
#include "GameInstance.h"
#include "Collider.h"

CFlyCollider_Boss::CFlyCollider_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject { pDevice, pContext }
{
}

CFlyCollider_Boss::CFlyCollider_Boss(const CFlyCollider_Boss& Prototype)
	: CPartObject { Prototype }
{
}

HRESULT CFlyCollider_Boss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlyCollider_Boss::Initialize(void* pArg)
{
	FLYCOLLIDER_BOSS_DESC* pDesc = static_cast<FLYCOLLIDER_BOSS_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eLayer = LAYER::WEAPON;
	//m_bIsCollisionEnabled = true;

	return S_OK;
}

void CFlyCollider_Boss::Update_Priority(_float fTimeDelta)
{
}

void CFlyCollider_Boss::Update(_float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (_uint i = 0; i < 3; i++)
		// 스케일 값을 정규화한다.
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	_matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentMatrix);

	__super::SetUp_CombinedWorldMatrix(ParentMatrix);

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMartix));
}

void CFlyCollider_Boss::Update_Late(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG

	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CFlyCollider_Boss::Render()
{
	//m_bIsCollisionEnabled = true;
#ifdef _DEBUG
	if (m_bIsCollisionEnabled == true)
		m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CFlyCollider_Boss::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

void CFlyCollider_Boss::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
	}
}

HRESULT CFlyCollider_Boss::Ready_Components()
{
	/* For Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
	SphereDesc.fRadius = 5.5f;
	SphereDesc.vCenter = _float3(-3.f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

CFlyCollider_Boss* CFlyCollider_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlyCollider_Boss* pInstance = new CFlyCollider_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFlyCollider_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFlyCollider_Boss::Clone(void* pArg)
{
	CFlyCollider_Boss* pInstance = new CFlyCollider_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFlyCollider_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlyCollider_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}