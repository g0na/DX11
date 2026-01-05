#include "UI_Shield.h"
#include "GameInstance.h"

USING(Client)

CUI_Shield::CUI_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CUI_Shield::CUI_Shield(const CUI_Shield& Prototype)
	: CUIObj { Prototype }
{
}

HRESULT CUI_Shield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Shield::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	CUIObj::UIOBJ_DESC        Desc{};

	Desc.fX = g_iWinSizeX * 0.095f;
	Desc.fY = g_iWinSizeY * 0.75f;
	Desc.fSizeX = 118;
	Desc.fSizeY = 175;
	lstrcpy(Desc.szName, TEXT("UI_Shield"));
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_bVisible = false;

	return S_OK;
}

void CUI_Shield::Update_Priority(_float fTimeDelta)
{
}

void CUI_Shield::Update(_float fTimeDelta)
{
}

void CUI_Shield::Update_Late(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pGameInstance->Add_RenderObject(RENDERGROUP::UI, this);
}

HRESULT CUI_Shield::Render()
{
	// 직교 투영을 위해 쉐이더에 행렬을 전달해주는 과정!!!!!!!!!!!!!!!!!!!!!!!!!
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shield::Ready_Components()
{
	// Com_VIBuffer 추가
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	// Com_Shader 추가
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Com_Texture 추가
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Shield"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shield::Bind_ShaderResources()
{
	// 직교투영용 뷰, 투영 행렬을 쉐이더에 전달한다.
	if (FAILED(__super::Bind_OrthoMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	// 직교투영용 월드 행렬을 쉐이더에 전달한다.
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// 텍스쳐를 쉐이더에 전달한다.
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_Shield* CUI_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Shield* pInstance = new CUI_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Shield::Clone(void* pArg)
{
	CUI_Shield* pInstance = new CUI_Shield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_Shield");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CUI_Shield::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
