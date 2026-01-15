#include "Item_Key.h"
#include "ContainerObject.h"
#include "GameInstance.h"

CItem_Key::CItem_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CItem_Key::CItem_Key(const CItem_Key& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CItem_Key::Initialize_Prototype()
{
	m_eLayer = LAYER::OBJECT;

	return S_OK;
}

HRESULT CItem_Key::Initialize(void* pArg)
{
	lstrcpy(m_szName, TEXT("Item_Key"));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	_vector vPosition = XMVectorSet(-11.81f, -15.24f, -45.8f, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	m_bIsCollisionEnabled = true;

	return S_OK;
}

void CItem_Key::Update_Priority(_float fTimeDelta)
{
	if (m_bVisible == false)
		m_fTimeElapsed += fTimeDelta;

	if (m_fTimeElapsed >= 3.f)
	{
		m_bIsActive = false;
		m_pGameInstance->Hide_UI(TEXT("Prototype_UI_PopUp_Key"));
	}

	if (m_pGameInstance->Get_KeyDown(DIK_E) && m_bIsColliding)
	{
		m_bVisible = false;
		m_pGameInstance->Show_UI(TEXT("Prototype_UI_Key"));
		m_pGameInstance->Show_UI(TEXT("Prototype_UI_PopUp_Key"));
	}
}

void CItem_Key::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CItem_Key::Update_Late(_float fTimeDelta)
{
	if (m_bVisible)
		m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CItem_Key::Render()
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

void CItem_Key::OnCollisionEnter(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
		m_pGameInstance->Show_UI(TEXT("Prototype_UI_PopUp_PickUp"));
		m_bIsColliding = true;
	}
}

void CItem_Key::OnCollisionExit(CGameObject* pOtherObject)
{
	if (pOtherObject->Get_Layer() == TEXT("Layer_Player"))
	{
		m_pGameInstance->Hide_UI(TEXT("Prototype_UI_PopUp_PickUp"));
		m_bIsColliding = false;
		m_bIsCollisionEnabled = false;
	}
}

HRESULT CItem_Key::Ready_Components()
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

HRESULT CItem_Key::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CItem_Key* CItem_Key::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Key* pInstance = new CItem_Key(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_Key");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Key::Clone(void* pArg)
{
	CItem_Key* pInstance = new CItem_Key(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_Key");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Key::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
