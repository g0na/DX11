#include "Estus.h"
#include "GameInstance.h"

CEstus::CEstus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CEstus::CEstus(const CEstus& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CEstus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEstus::Initialize(void* pArg)
{
	ESTUS_DESC* pDesc = static_cast<ESTUS_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMConvertToRadians(-90.0f), 0.f, 0.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.1f, 0.05f, 0.1f, 1.f));

	m_bIsActive = false;

	return S_OK;
}

void CEstus::Update_Priority(_float fTimeDelta)
{
}

void CEstus::Update(_float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (_uint i = 0; i < 3; i++)
		// 스케일 값을 정규화한다.
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	_matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentMatrix);

	__super::SetUp_CombinedWorldMatrix(ParentMatrix);
}

void CEstus::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CEstus::Render()
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

	return S_OK;
}

HRESULT CEstus::Ready_Components()
{
	/* For Com_Model */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Estus"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEstus::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMartix)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CEstus* CEstus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEstus* pInstance = new CEstus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEstus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEstus::Clone(void* pArg)
{
	CEstus* pInstance = new CEstus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEstus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEstus::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}