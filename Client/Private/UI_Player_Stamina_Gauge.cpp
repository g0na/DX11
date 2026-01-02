#include "UI_Player_Stamina_Gauge.h"
#include "GameInstance.h"

USING(Client)

CUI_Player_Stamina_Gauge::CUI_Player_Stamina_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CUI_Player_Stamina_Gauge::CUI_Player_Stamina_Gauge(const CUI_Player_Stamina_Gauge& Prototype)
	: CUIObj { Prototype }
{
}

HRESULT CUI_Player_Stamina_Gauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Player_Stamina_Gauge::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	CUIObj::UIOBJ_DESC        Desc{};

	Desc.fX = g_iWinSizeX * 0.33f;
	Desc.fY = g_iWinSizeY * 0.1f + 22.f;
	Desc.fSizeX = 800.f;
	Desc.fSizeY = 30.f;
	lstrcpy(Desc.szName, TEXT("UI_Player_Stamina_Gauge"));
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Player_Stamina_Gauge::Update_Priority(_float fTimeDelta)
{
}

void CUI_Player_Stamina_Gauge::Update(_float fTimeDelta)
{
}

void CUI_Player_Stamina_Gauge::Update_Late(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pGameInstance->Add_RenderObject(RENDERGROUP::UI, this);
}

HRESULT CUI_Player_Stamina_Gauge::Render()
{
	// 직교 투영을 위해 쉐이더에 행렬을 전달해주는 과정!!!!!!!!!!!!!!!!!!!!!!!!!
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Player_Stamina_Gauge::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Player_Gauge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Player_Stamina_Gauge::Bind_ShaderResources()
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

CUI_Player_Stamina_Gauge* CUI_Player_Stamina_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Player_Stamina_Gauge* pInstance = new CUI_Player_Stamina_Gauge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Player_Stamina_Gauge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Player_Stamina_Gauge::Clone(void* pArg)
{
	CUI_Player_Stamina_Gauge* pInstance = new CUI_Player_Stamina_Gauge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_Player_Stamina_Gauge");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CUI_Player_Stamina_Gauge::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
