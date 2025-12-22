#include "Player_HealStart.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"

CPlayer_HealStart::CPlayer_HealStart()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_HealStart::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;

    if (m_pStateMachine == nullptr ||
        m_pPlayerBody == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);

    return S_OK;
}

void CPlayer_HealStart::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(19, false);
}

void CPlayer_HealStart::Update_State(_float fTimeDelta)
{
    // »ç¸Á
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Dead"), false) == true)
        m_pStateMachine->Change_State(CPlayer::DEATH);

    // ÇÇ°Ý
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false))
        m_pStateMachine->Change_State(CPlayer::DAMAGED);

    // ¿¡½ºÆ® ¸Ô±â
    if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::HEAL_ING);
}

void CPlayer_HealStart::Exit_State()
{
}

CPlayer_HealStart* CPlayer_HealStart::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_HealStart* pInstance = new CPlayer_HealStart();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_HealStart");
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CPlayer_HealStart::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
