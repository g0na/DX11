#include "Weapon.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Monster_Darkwraith.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject { pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
	: CPartObject { Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMConvertToRadians(90.0f), 0.f, 0.f);

	m_eLayer = LAYER::WEAPON;
	m_bIsCollisionEnabled = false;

	return S_OK;
}

void CWeapon::Update_Priority(_float fTimeDelta)
{
}

void CWeapon::Update(_float fTimeDelta)
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

void CWeapon::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
	for (_uint i = 0; i < m_iColliderCnt; i++)
	{
		m_pGameInstance->Add_DebugComponent(m_vecColliders[i]);
	}
#endif // _DEBUG
}

HRESULT CWeapon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	if (m_bIsCollisionEnabled == true)
	{
		for (auto& pCollider : m_vecColliders)
			pCollider->Render();
	}

	return S_OK;
}

void CWeapon::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Monster"))
	{
		if (dynamic_cast<CMonster_Darkwraith*>(pOtherObject) != nullptr)
			dynamic_cast<CMonster_Darkwraith*>(pOtherObject)->Set_Damaged(true);
	}
}

void CWeapon::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Monster"))
	{
		if (dynamic_cast<CMonster_Darkwraith*>(pOtherObject) != nullptr)
		dynamic_cast<CMonster_Darkwraith*>(pOtherObject)->Set_Damaged(false);
	}
}

HRESULT CWeapon::Ready_Components()
{
	/* For Com_Model */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Player_Weapon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For Com_Collider */
	for (_uint i = 0; i < m_iColliderCnt; i++)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
		SphereDesc.fRadius = 0.1f;
		_float fOffset = SphereDesc.fRadius - 0.9f + (0.2f * i);
		SphereDesc.vCenter = _float3(0.f, fOffset, 0.f);

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_TO_UINT(LEVELID::GAMEPLAY),
			TEXT("Prototype_Component_Collider_Sphere"), &SphereDesc));

		m_vecColliders.push_back(pCollider);
	}
		
	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMartix)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	for (auto& pCollider : m_vecColliders)
		Safe_Release(pCollider);
	m_vecColliders.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}