#include "Background_Loading.h"
#include "GameInstance.h"

USING(Client)

CBackground_Loading::CBackground_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CBackground_Loading::CBackground_Loading(const CBackground_Loading& Prototype)
	: CUIObj { Prototype }
{
}

HRESULT CBackground_Loading::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackground_Loading::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	CUIObj::UIOBJ_DESC        Desc{};

	Desc.fX = g_iWinSizeX >> 3;
	Desc.fY = g_iWinSizeY * 0.8f;
	Desc.fSizeX = 256.f;
	Desc.fSizeY = 256.f;
	lstrcpy(Desc.szName, TEXT("BackGround_Loading"));
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vScaleUV = _float2(m_fSizeX, m_fSizeY);

	return S_OK;
}

void CBackground_Loading::Update_Priority(_float fTimeDelta)
{
}

void CBackground_Loading::Update(_float fTimeDelta)
{
	m_fFrameDelay += fTimeDelta;

	if (m_fFrameDelay >= 0.025f)
	{
		m_vOffsetUV = _float2((m_iCurrentFrameIdx % m_iCountX) * m_fSizeX,
							  (m_iCurrentFrameIdx / m_iCountX) * m_fSizeY);

		m_iCurrentFrameIdx++;
		m_fFrameDelay = 0.f;
	}

	if (m_iCurrentFrameIdx >= 32)
		m_iCurrentFrameIdx = 0;
}

void CBackground_Loading::Update_Late(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pGameInstance->Add_RenderObject(RENDERGROUP::UI, this);
}

HRESULT CBackground_Loading::Render()
{
	// 직교 투영을 위해 쉐이더에 행렬을 전달해주는 과정!!!!!!!!!!!!!!!!!!!!!!!!!
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackground_Loading::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::STATIC), TEXT("Prototype_Component_Texture_Loading"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackground_Loading::Bind_ShaderResources()
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
	
	// UV 크기, UV Offset 전달하기	g_vOffsetUV, g_vScaleUV
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOffsetUV", &m_vOffsetUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vScaleUV", &m_vScaleUV, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CBackground_Loading* CBackground_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackground_Loading* pInstance = new CBackground_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackground_Loading::Clone(void* pArg)
{
	CBackground_Loading* pInstance = new CBackground_Loading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBackGround");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CBackground_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
