#include "GameObject.h"
#include "GameInstance.h"

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
	, m_eLayer {prototype.m_eLayer}
	, m_bIsDead { prototype.m_bIsDead }
	, m_bIsGrounded { prototype.m_bIsGrounded }
	, m_bIsInvincible { prototype.m_bIsInvincible }
	, m_strPrototypeTag { prototype.m_strPrototypeTag }
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
	// 트랜스폼 컴포넌트를 생성한다.
	// pArg가 nullptr 이어도 트랜스폼은 무조건 필요하므로 트랜스폼을 생성한다.
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (m_pTransformCom == nullptr)
		return E_FAIL;
	
	// nullptr가 들어왔을 경우 예외처리
	if (pArg != nullptr)
	{
		GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

		lstrcpy(m_szName, pDesc->szName);
	}

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

const _wstring CGameObject::Get_Layer() const
{
	switch (m_eLayer)
	{
	case LAYER::PLAYER:
		return TEXT("Layer_Player");

	case LAYER::MONSTER:
		return TEXT("Layer_Monster");

	case LAYER::OBJECT:
		return TEXT("Layer_Object");

	case LAYER::MAP:
		return TEXT("Layer_Map");

	case LAYER::CAMERA:
		return TEXT("Layer_Camera");

	default:
		MSG_BOX("Failed to get layer!");
		return TEXT("INVALID_LAYER");
	}
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelID, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	// 오브젝트에 컴포넌트 복제본이 있는지 확인하여 예외처리를 한다.
	auto iter = m_mapComponents.find(strComponentTag);
	if (iter != m_mapComponents.end())
		return E_FAIL;

	// 복제본이 없는 것을 확인한 후 복제하여 오브젝트의 map 컨테이너에 삽입한다.
	CComponent* pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelID, strPrototypeTag, pArg));
	if (pComponent == nullptr)
		return E_FAIL;

	m_mapComponents.emplace(strComponentTag, pComponent);

	// 오브젝트 자체가 가지고 있는 멤버 변수에도 삽입하고 컴포넌트를 참조하였으므로 레퍼런스 카운트를 증가시킨다.
	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	// 컴포넌트를 담고있는 map 컨테이너를 비워준다.
	for (auto& Pair : m_mapComponents)
		Safe_Release(Pair.second);
	m_mapComponents.clear();

	Safe_Release(m_pStateMachine);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}