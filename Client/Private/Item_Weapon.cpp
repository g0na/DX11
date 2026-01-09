#include "Item_Weapon.h"
#include "Weapon.h"
#include "ContainerObject.h"
#include "GameInstance.h"

CItem_Weapon::CItem_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CItem_Weapon::CItem_Weapon(const CItem_Weapon& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CItem_Weapon::Initialize_Prototype()
{
	m_eLayer = LAYER::OBJECT;

	return S_OK;
}

HRESULT CItem_Weapon::Initialize(void* pArg)
{
	lstrcpy(m_szName, TEXT("Item_Weapon"));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayerWeapon = static_cast<CWeapon*>((static_cast<CContainerObject*>(m_pGameInstance->Get_Player(ENUM_TO_UINT(LEVELID::GAMEPLAY)))->Find_PartObject(TEXT("Part_Weapon"))));

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	_vector vPosition = XMVectorSet(32.98000717163086f, -7.8175435066223145f, -12.387142181396484f, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	m_bIsCollisionEnabled = true;

	return S_OK;
}

void CItem_Weapon::Update_Priority(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_E) && m_bIsColliding)
	{
		m_bIsActive = false;
		m_pGameInstance->Show_UI(TEXT("Prototype_UI_Weapon"));
		m_pPlayerWeapon->Set_Activity(true);
	}
}

void CItem_Weapon::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CItem_Weapon::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CItem_Weapon::Render()
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

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CItem_Weapon::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
		m_pGameInstance->Show_UI(TEXT("Prototype_UI_PopUp_PickUp"));
		m_bIsColliding = true;
	}
}

void CItem_Weapon::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
		m_pGameInstance->Hide_UI(TEXT("Prototype_UI_PopUp_PickUp"));
		m_bIsColliding = false;
		m_bIsCollisionEnabled = false;
	}
}

HRESULT CItem_Weapon::Ready_Components()
{
	// For Com_Model
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Item"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// For Com_Shader
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// For_Com_Collider
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
	SphereDesc.fRadius = 1.f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CItem_Weapon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CItem_Weapon* CItem_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Weapon* pInstance = new CItem_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Weapon::Clone(void* pArg)
{
	CItem_Weapon* pInstance = new CItem_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
