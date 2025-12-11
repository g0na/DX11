#include "Player_Recoil.h"
#include "Player.h"
#include "Body.h"

CPlayer_Recoil::CPlayer_Recoil()
{
}

HRESULT CPlayer_Recoil::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    Safe_AddRef(m_pPlayerBody);

    if (m_pStateMachine == nullptr ||
        m_pPlayerBody == nullptr)
        return E_FAIL;

    return S_OK;
}

void CPlayer_Recoil::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(2, false);

    m_pStateMachine->Set_BoolData(TEXT("Player_Recoil"), false);
}

void CPlayer_Recoil::Update_State(_float fTimeDelta)
{
    // 막다가 공격 또 들어오면 다시
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Recoil"), false) == true)
    {
        Enter_State();
        return;
    }

    if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::GUARD);
}

void CPlayer_Recoil::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Player_Recoil"), false);
}

CPlayer_Recoil* CPlayer_Recoil::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Recoil* pInstance = new CPlayer_Recoil();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Recoil");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Recoil::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerBody);
}
