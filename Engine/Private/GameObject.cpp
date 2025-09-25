#include "GameObject.h"
#include "UIObj.h"

USING(Engine)

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& prototype)
	: m_pDevice {prototype.m_pDevice}
	, m_pContext {prototype.m_pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	lstrcpy(m_szName, pDesc->szName);

	// 트랜스폼 컴포넌트를 생성한다.
	// 생성한 트랜스폼에게 pArg를 던져준다.

	return S_OK;
}

void CGameObject::Update_Priority(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{

}

void CGameObject::Update_Late(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}


void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}