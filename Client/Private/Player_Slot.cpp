#include "Player_Slot.h"
#include "GameInstance.h"

USING(Client)

CPlayer_Slot::CPlayer_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObj { pDevice, pContext }
{
}

CPlayer_Slot::CPlayer_Slot(const CPlayer_Slot& Prototype)
	: CUIObj { Prototype }
{
}

HRESULT CPlayer_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Slot::Initialize(void* pArg)
{
	// 여기서 초기화 할 때 정보를 설정해주거나, 사용할 레벨에서 정보를 설정해줘도 된다.
	CUIObj::UIOBJ_DESC        Desc{};

	Desc.fX = g_iWinSizeX * 0.15f;
	Desc.fY = g_iWinSizeY * 0.75f;
	Desc.fSizeX = 350;
	Desc.fSizeY = 280;
	lstrcpy(Desc.szName, TEXT("Player_Slot"));
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Slot::Update_Priority(_float fTimeDelta)
{
}

void CPlayer_Slot::Update(_float fTimeDelta)
{
}

void CPlayer_Slot::Update_Late(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pGameInstance->Add_RenderObject(RENDERGROUP::UI, this);
}

HRESULT CPlayer_Slot::Render()
{
	// 직교 투영을 위해 쉐이더에 행렬을 전달해주는 과정!!!!!!!!!!!!!!!!!!!!!!!!!
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Slot::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_PlayerSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Slot::Bind_ShaderResources()
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

CPlayer_Slot* CPlayer_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Slot* pInstance = new CPlayer_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Slot::Clone(void* pArg)
{
	CPlayer_Slot* pInstance = new CPlayer_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer_Slot");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CPlayer_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
