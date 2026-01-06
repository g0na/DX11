#include "InDoor.h"
#include "GameInstance.h"

CInDoor::CInDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CInDoor::CInDoor(const CInDoor& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CInDoor::Initialize_Prototype()
{
	m_eLayer = LAYER::OBJECT;

	return S_OK;
}

HRESULT CInDoor::Initialize(void* pArg)
{
	lstrcpy(m_szName, TEXT("InDoor"));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, false);

	return S_OK;
}

void CInDoor::Update_Priority(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_P))
		m_pModelCom->Set_Animation(0, false);
	else if (m_pGameInstance->Get_KeyDown(DIK_O))
		m_pModelCom->Set_Animation(1, false);
}

void CInDoor::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CInDoor::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CInDoor::Render()
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

	return S_OK;
}

HRESULT CInDoor::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_InDoor"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// For Com_Shader
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInDoor::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CInDoor* CInDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInDoor* pInstance = new CInDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInDoor::Clone(void* pArg)
{
	CInDoor* pInstance = new CInDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInDoor::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
