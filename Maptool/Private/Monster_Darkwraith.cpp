#include "Monster_Darkwraith.h"
#include "GameInstance.h"

CMonster_Darkwraith::CMonster_Darkwraith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster_Darkwraith::CMonster_Darkwraith(const CMonster_Darkwraith& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMonster_Darkwraith::Initialize_Prototype()
{
	m_eLayer = LAYER::MONSTER;

	return S_OK;
}

HRESULT CMonster_Darkwraith::Initialize(void* pArg)
{
	lstrcpy(m_szName, TEXT("Darkwraith"));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, true);

	return S_OK;
}

void CMonster_Darkwraith::Update_Priority(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_1))
		m_pModelCom->Set_Animation(0, true);
	else if (m_pGameInstance->Get_KeyDown(DIK_2))
		m_pModelCom->Set_Animation(1, true);
	else if (m_pGameInstance->Get_KeyDown(DIK_3))
		m_pModelCom->Set_Animation(2, true);
}

void CMonster_Darkwraith::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	_vector vRootMotionDelta = m_pModelCom->Get_RootMotionDelta();

	_float3	fDelta;
	XMStoreFloat3(&fDelta, vRootMotionDelta);

	// 로컬 공간의 루트 모션 델타를 월드 공간의 벡터로 바꿔줘야 내가 바꾼 회전이 적용댐!
	_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
	_vector vUp = m_pTransformCom->Get_State(STATE::UP);
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	_vector vWorldDelta = vRight * fDelta.x + vUp * fDelta.y + vLook * fDelta.z;
	
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	vPosition += vWorldDelta;
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
}

void CMonster_Darkwraith::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::BLEND, this);
}

HRESULT CMonster_Darkwraith::Render()
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

HRESULT CMonster_Darkwraith::Ready_Components()
{
	// For Com_Model
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Darkwraith"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// For Com_Shader
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Darkwraith::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

CMonster_Darkwraith* CMonster_Darkwraith::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Darkwraith* pInstance = new CMonster_Darkwraith(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Darkwraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Darkwraith::Clone(void* pArg)
{
	CMonster_Darkwraith* pInstance = new CMonster_Darkwraith(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Darkwraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Darkwraith::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
