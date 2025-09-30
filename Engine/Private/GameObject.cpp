#include "GameObject.h"
#include "GameInstance.h"

USING(Engine)

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
;}

CGameObject::CGameObject(const CGameObject& prototype)
	: m_pDevice {prototype.m_pDevice}
	, m_pContext {prototype.m_pContext}
	, m_pGameInstance {prototype.m_pGameInstance}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
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
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	// 생성한 트랜스폼에게 pArg 그대로를 던져준다.
	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;
	
	// 생성한 컴포넌트를 컨테이너에 넣어준다.
	// 컨테이너에 추가할 때 레퍼런스 카운트를 올려준다.
	m_mapComponents.emplace(g_strTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);

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

	// 컴포넌트를 담고있는 map 컨테이너를 비워준다.
	for (auto& Pair : m_mapComponents)
		Safe_Release(Pair.second);
	m_mapComponents.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}