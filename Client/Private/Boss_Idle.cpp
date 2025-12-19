#include "Boss_Idle.h"
#include "Monster_Boss.h"
#include "GameInstance.h"

CBoss_Idle::CBoss_Idle()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_Idle::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr)
        return E_FAIL;

    return S_OK;
}

void CBoss_Idle::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(0, true);
}

void CBoss_Idle::Update_State(_float fTimeDelta)
{
    if (m_pStateMachine->Get_BoolData(TEXT("Boss_Targeting"), false) == true)
        m_pStateMachine->Change_State(CMonster_Boss::WALK);
    else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 6.f)
    {
        if (m_pGameInstance->Random(1.f, 10.f) >= 6.f)
            m_pStateMachine->Change_State(CMonster_Boss::FLYATTACK);
        else
            m_pStateMachine->Change_State(CMonster_Boss::BACKSTEP);
    }
    else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 14.f)
        m_pStateMachine->Change_State(CMonster_Boss::ATTACK);
    else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 18.f)
        m_pStateMachine->Change_State(CMonster_Boss::DASHATTACK);
}

void CBoss_Idle::Exit_State()
{
}

CBoss_Idle* CBoss_Idle::Create(CGameObject* pOwner)
{
    CBoss_Idle* pInstance = new CBoss_Idle();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_Idle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Idle::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
