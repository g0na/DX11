#include "UIObj.h"
#include "Shader.h"

CUIObj::CUIObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject {pDevice, pContext}
{
}

CUIObj::CUIObj(const CUIObj& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CUIObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObj::Initialize(void* pArg)
{
	UIOBJ_DESC* pDesc = static_cast<UIOBJ_DESC*>(pArg);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 윈도우 사이즈를 가져오기 위한 뷰포트 함수
	D3D11_VIEWPORT		ViewPortDesc{};
	_uint				iNumViewPorts = { 1 };

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

	// 직교투영을 위한 월드, 뷰, 투영행렬을 세팅한다.
	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_fX - ViewPortDesc.Width * 0.5f,
		-m_fY + ViewPortDesc.Height * 0.5f,
		0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

	return S_OK;
}

void CUIObj::Update_Priority(_float fTimeDelta)
{
}

void CUIObj::Update(_float fTimeDelta)
{
}

void CUIObj::Update_Late(_float fTimeDelta)
{
}

HRESULT CUIObj::Render()
{
	return S_OK;
}

HRESULT CUIObj::Bind_OrthoMatrices(class CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName)
{
	if (FAILED(pShader->Bind_Matrix(pViewMatrixName, &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix(pProjMatrixName, &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CUIObj::Free()
{
	__super::Free();

}
