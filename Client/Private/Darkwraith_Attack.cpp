#include "Darkwraith_Attack.h"
#include "Transform.h"
#include "Monster_Darkwraith.h"
#include "GameInstance.h"

CDarkwraith_Attack::CDarkwraith_Attack()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDarkwraith_Attack::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr)
        return E_FAIL;

    return S_OK;
}

void CDarkwraith_Attack::Enter_State()
{
    if (m_pOwner != nullptr)
        dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(4, false);
}

void CDarkwraith_Attack::Update_State(_float fTimeDelta)
{
    m_fAttackDelay += fTimeDelta;

    _vector vPlayerPos = dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Get_PlayerPos();
    m_pMonsterTransform->LookAt(vPlayerPos);

    if (m_fAttackDelay >= 1.5f &&
        dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Get_TargetDist() > 3.f)
        m_pStateMachine->Change_State(CMonster_Darkwraith::WALK);
}

void CDarkwraith_Attack::Exit_State()
{
    m_fAttackDelay = 0.f;
}

CDarkwraith_Attack* CDarkwraith_Attack::Create(CGameObject* pOwner)
{
    CDarkwraith_Attack* pInstance = new CDarkwraith_Attack();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CDarkwraith_Attack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDarkwraith_Attack::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
