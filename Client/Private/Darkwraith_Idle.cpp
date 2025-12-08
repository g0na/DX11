#include "Darkwraith_Idle.h"
#include "Transform.h"
#include "Monster_Darkwraith.h"
#include "GameInstance.h"

CDarkwraith_Idle::CDarkwraith_Idle()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDarkwraith_Idle::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr)
        return E_FAIL;

    return S_OK;
}

void CDarkwraith_Idle::Enter_State()
{
    if (m_pOwner != nullptr)
        dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(0, true);
}

void CDarkwraith_Idle::Update_State(_float fTimeDelta)
{
    if (m_pStateMachine->Get_BoolData(TEXT("Darkwraith_Targeting"), false))
        m_pStateMachine->Change_State(CMonster_Darkwraith::WALK);
    else if (m_pStateMachine->Get_BoolData(TEXT("Darkwraith_Damaged"), false))
        m_pStateMachine->Change_State(CMonster_Darkwraith::DAMAGED);
}

void CDarkwraith_Idle::Exit_State()
{
}

CDarkwraith_Idle* CDarkwraith_Idle::Create(CGameObject* pOwner)
{
    CDarkwraith_Idle* pInstance = new CDarkwraith_Idle();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CDarkwraith_Idle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDarkwraith_Idle::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
