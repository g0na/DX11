#include "UI_Boss_HP.h"
#include "GameInstance.h"
#include "Monster_Boss.h"

USING(Client)

CUI_Boss_HP::CUI_Boss_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CUI_Boss_HP::CUI_Boss_HP(const CUI_Boss_HP& Prototype)
	: CUIObj { Prototype }
{
}

HRESULT CUI_Boss_HP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Boss_HP::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	CUIObj::UIOBJ_DESC        Desc{};

	Desc.fX = g_iWinSizeX * 0.608f;
	Desc.fY = g_iWinSizeY * 0.872f;
	Desc.fSizeX = 1110.f;
	Desc.fSizeY = 20.f;
	lstrcpy(Desc.szName, TEXT("UI_Boss_HP"));
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pBoss = static_cast<CMonster_Boss*>(m_pGameInstance->Get_Object(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Layer_Monster"), TEXT("Boss")));
	if (m_pBoss == nullptr)
		return E_FAIL;
	Safe_AddRef(m_pBoss);

	m_fRatio = 1.f;
	m_bVisible = false;

	return S_OK;
}

void CUI_Boss_HP::Update_Priority(_float fTimeDelta)
{
}

void CUI_Boss_HP::Update(_float fTimeDelta)
{
	m_fRatio = m_pBoss->Get_HpRatio();
}

void CUI_Boss_HP::Update_Late(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pGameInstance->Add_RenderObject(RENDERGROUP::UI, this);
}

HRESULT CUI_Boss_HP::Render()
{
	// 직교 투영을 위해 쉐이더에 행렬을 전달해주는 과정!!!!!!!!!!!!!!!!!!!!!!!!!
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGameInstance->Draw_Text(TEXT("Font_Korean"), TEXT("수용소의 데몬"), _float2(g_iWinSizeX * 0.315f, g_iWinSizeY * 0.82f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CUI_Boss_HP::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Player_HP"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Boss_HP::Bind_ShaderResources()
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

	// 체력 비율 쉐이더에 전달
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_Boss_HP* CUI_Boss_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Boss_HP* pInstance = new CUI_Boss_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Boss_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Boss_HP::Clone(void* pArg)
{
	CUI_Boss_HP* pInstance = new CUI_Boss_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_Boss_HP");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CUI_Boss_HP::Free()
{
	__super::Free();

	Safe_Release(m_pBoss);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
