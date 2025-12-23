#include "Player_HealStart.h"
#include "Player.h"
#include "Body.h"
#include "Weapon.h"
#include "Estus.h"
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
    m_pEstus = static_cast<CEstus*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Estus")));
    m_pWeapon = static_cast<CWeapon*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Weapon")));

    if (m_pStateMachine == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pEstus == nullptr ||
        m_pWeapon == nullptr)
        return E_FAIL;

    return S_OK;
}

void CPlayer_HealStart::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(19, false);
}

void CPlayer_HealStart::Update_State(_float fTimeDelta)
{
    m_fHealTime += fTimeDelta;

    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Dead"), false) == true)
        m_pStateMachine->Change_State(CPlayer::DEATH);

    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false))
        m_pStateMachine->Change_State(CPlayer::DAMAGED);

    // 에스트병 활성화, 무기 비활성화
    if (m_fHealTime >= 0.166f)
    {
        m_pWeapon->Set_Activity(false);
        m_pEstus->Set_Activity(true);
    }

    // 에스트 먹기
    if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::HEAL_ING);
}

void CPlayer_HealStart::Exit_State()
{
    m_fHealTime = 0.f;
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
