#include "Background.h"
#include "GameInstance.h"

USING(Client)

CBackground::CBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CBackground::CBackground(const CBackground& Prototype)
	: CUIObj { Prototype }
{
}

HRESULT CBackground::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackground::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	CUIObj::UIOBJ_DESC        Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	lstrcpy(Desc.szName, TEXT("BackGround"));
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackground::Update_Priority(_float fTimeDelta)
{
	int a = 10;
}

void CBackground::Update(_float fTimeDelta)
{
	int a = 10;
}

void CBackground::Update_Late(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(RENDERGROUP::UI, this);
}

HRESULT CBackground::Render()
{
#pragma region 원근 투영
	// 원근 투영을 위해 쉐이더에 행렬을 전달해주는 과정
	// 임의로 월드, 뷰, 투영 행렬을 만들어서 전달했다.
	//_float4x4		WorldMat, ViewMat, ProjMat;

	//XMStoreFloat4x4(&WorldMat, XMMatrixIdentity());
	//// 계산된 뷰 행렬을 출력 (Eye, Look, Up)
	//XMStoreFloat4x4(&ViewMat,
	//	XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, -3.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	//// 원근투영 행렬을 돌려준다. (수직 시야각, 종횡비 = 너비 / 높이, 가까운 평면까지의 거리, 먼 평면까지의 거리)
	//XMStoreFloat4x4(&ProjMat,
	//	XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), static_cast<_float>(g_iWinSizeX) / g_iWinSizeY, 0.1f, 500.f));

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
	//	return E_FAIL;
#pragma endregion

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

HRESULT CBackground::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::LOGO), TEXT("Prototype_Component_Texture_Background"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackground::Bind_ShaderResources()
{
	// 직교투영용 뷰, 투영 행렬을 쉐이더에 전달한다.
	if (FAILED(__super::Bind_OrthoMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	// 직교투영용 월드 행렬을 쉐이더에 전달한다.
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// 텍스쳐를 쉐이더에 전달한다.
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CBackground* CBackground::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackground* pInstance = new CBackground(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackground::Clone(void* pArg)
{
	CBackground* pInstance = new CBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBackGround");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CBackground::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
