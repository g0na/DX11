#include "UI_Estus.h"
#include "GameInstance.h"

USING(Client)

CUI_Estus::CUI_Estus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CUI_Estus::CUI_Estus(const CUI_Estus& Prototype)
	: CUIObj { Prototype }
{
}

void CUI_Estus::Set_EstusCount()
{
	if (m_iEstusCount <= 0)
		return;

	m_iEstusCount--;

	swprintf_s(m_szEstusCount, _countof(m_szEstusCount), L"%d", m_iEstusCount);
}

HRESULT CUI_Estus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Estus::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	CUIObj::UIOBJ_DESC        Desc{};

	Desc.fX = g_iWinSizeX * 0.175f;
	Desc.fY = g_iWinSizeY * 0.84f;
	Desc.fSizeX = 136;
	Desc.fSizeY = 170;
	lstrcpy(Desc.szName, TEXT("UI_Estus"));
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iTextureIndex = 0;
	m_iEstusCount = 10;
	swprintf_s(m_szEstusCount, _countof(m_szEstusCount), L"%d", m_iEstusCount);

	return S_OK;
}

void CUI_Estus::Update_Priority(_float fTimeDelta)
{
	if (m_iEstusCount <= 0)
		m_iTextureIndex = 1;
}

void CUI_Estus::Update(_float fTimeDelta)
{
}

void CUI_Estus::Update_Late(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pGameInstance->Add_RenderObject(RENDERGROUP::UI, this);
}

HRESULT CUI_Estus::Render()
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

	m_pGameInstance->Draw_Text(TEXT("Font_Korean"), m_szEstusCount, _float2(g_iWinSizeX * 0.195f, g_iWinSizeY * 0.9f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CUI_Estus::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Estus"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Estus::Bind_ShaderResources()
{
	// 직교투영용 뷰, 투영 행렬을 쉐이더에 전달한다.
	if (FAILED(__super::Bind_OrthoMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	// 직교투영용 월드 행렬을 쉐이더에 전달한다.
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// 텍스쳐를 쉐이더에 전달한다.
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

CUI_Estus* CUI_Estus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Estus* pInstance = new CUI_Estus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Estus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Estus::Clone(void* pArg)
{
	CUI_Estus* pInstance = new CUI_Estus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_Estus");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CUI_Estus::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
