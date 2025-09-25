#include "UIObj.h"

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

	m_fX = pDesc->m_fX;
	m_fY = pDesc->m_fY;
	m_fSizeX = pDesc->m_fSizeX;
	m_fSizeY = pDesc->m_fSizeY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

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

void CUIObj::Free()
{
	__super::Free();

}
