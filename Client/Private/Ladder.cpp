#include "Ladder.h"
#include "GameInstance.h"

CLadder::CLadder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CLadder::CLadder(const CLadder& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CLadder::Initialize_Prototype()
{
	m_eLayer = LAYER::OBJECT;

	return S_OK;
}

HRESULT CLadder::Initialize(void* pArg)
{
	lstrcpy(m_szName, TEXT("Ladder"));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bIsCollisionEnabled = true;

	return S_OK;
}

void CLadder::Update_Priority(_float fTimeDelta)
{
}

void CLadder::Update(_float fTimeDelta)
{
	for (auto& pCollider : m_vecColliders)
		pCollider->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CLadder::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CLadder::Render()
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
	if (m_bIsCollisionEnabled == true)
	{
		for (auto& pCollider : m_vecColliders)
			pCollider->Render();
	}
#endif

	return S_OK;
}

void CLadder::OnCollisionEnter(CGameObject* pOtherObject)
{
}

void CLadder::OnCollisionExit(CGameObject* pOtherObject)
{
}

HRESULT CLadder::Ready_Components()
{
	// For Com_Model
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Ladder"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// For Com_Shader
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// For Com_Collider
	for (_uint i = 0; i < m_iColliderCnt; i++)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
		SphereDesc.fRadius = 0.8f;
		_float fOffset = SphereDesc.fRadius + 0.35f + (1.6f * i);
		SphereDesc.vCenter = _float3(0.f, fOffset, -0.4f);

		CCollider* pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_TO_UINT(LEVELID::GAMEPLAY),
			TEXT("Prototype_Component_Collider_Sphere"), &SphereDesc));

		m_vecColliders.push_back(pCollider);
	}

	return S_OK;
}

HRESULT CLadder::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CLadder* CLadder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLadder* pInstance = new CLadder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLadder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLadder::Clone(void* pArg)
{
	CLadder* pInstance = new CLadder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLadder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLadder::Free()
{
	__super::Free();

	for (auto& pCollider : m_vecColliders)
		Safe_Release(pCollider);
	m_vecColliders.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
