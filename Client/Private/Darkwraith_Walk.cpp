#include "Darkwraith_Walk.h"
#include "Transform.h"
#include "Monster_Darkwraith.h"
#include "GameInstance.h"

CDarkwraith_Walk::CDarkwraith_Walk()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDarkwraith_Walk::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr)
        return E_FAIL;

    return S_OK;
}

void CDarkwraith_Walk::Enter_State()
{
    if (m_pOwner != nullptr)
        dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(1, true);
}

void CDarkwraith_Walk::Update_State(_float fTimeDelta)
{
    _vector vPlayerPos = dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Get_PlayerPos();
    m_pMonsterTransform->LookAt(vPlayerPos);
    
    if (dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Get_TargetDist() <= 3.f)
        m_pStateMachine->Change_State(CMonster_Darkwraith::ATTACK);

    if (dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Is_Targeting() == false)
        m_pStateMachine->Change_State(CMonster_Darkwraith::IDLE);
}

void CDarkwraith_Walk::Exit_State()
{
    _vector vPlayerPos = dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Get_PlayerPos();
    m_pMonsterTransform->LookAt(vPlayerPos);
}

CDarkwraith_Walk* CDarkwraith_Walk::Create(CGameObject* pOwner)
{
    CDarkwraith_Walk* pInstance = new CDarkwraith_Walk();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CDarkwraith_Walk");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDarkwraith_Walk::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
