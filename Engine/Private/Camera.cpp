#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;

}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	/* 카메라또한 트랜스폼 컴포넌트를 붙인다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 초기 셋팅하고자하는 데이터를 트랜스폼에 동기화한다.(뷰행렬) */
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));


	D3D11_VIEWPORT          ViewportDesc{};
	_uint           iNumViewports = { 1 };

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* 투영행렬을 위한 데이터도 셋팅해나간다 */
	/* 멤버로 보관이유, 투영행렬또한 갱신될 수 있기 때문에 .*/
	m_fFovY = pDesc->fFovY;
	m_fAspect = ViewportDesc.Width / ViewportDesc.Height;
	m_fNearZ = pDesc->fNearZ;
	m_fFarZ = pDesc->fFarZ;

	/* 어떤 객체서든 접근 가능한 파이프라인 클래스에 저장한다. */
	m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNearZ, m_fFarZ));

	return S_OK;
}

void CCamera::Update_Priority(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{

}

void CCamera::Update_Late(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}


void CCamera::Update_PipeLine()
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNearZ, m_fFarZ));
}

void CCamera::Free()
{
	__super::Free();

}