#include "Player_Healing.h"
#include "Player.h"
#include "Body.h"
#include "Estus.h"
#include "Weapon.h"
#include "GameInstance.h"

CPlayer_Healing::CPlayer_Healing()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Healing::Initialize(CGameObject* pOwner, CBody* pBody)
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

void CPlayer_Healing::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(20, false);

    m_fCoolDown = 0.f;
}

void CPlayer_Healing::Update_State(_float fTimeDelta)
{
    m_fCoolDown += fTimeDelta;

    // »ç¸Á
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Dead"), false) == true)
        m_pStateMachine->Change_State(CPlayer::DEATH);

    // ÇÇ°Ý
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false))
        m_pStateMachine->Change_State(CPlayer::DAMAGED);

    // ¿¬¼ÓÀ¸·Î ¸Ô±â
    if (m_pGameInstance->Get_KeyDown(DIK_R))
    {
        if (m_fCoolDown >= 0.667f)
            Enter_State();
    }

    // ¿¡½ºÆ® ¸Ô±â
    if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::HEAL_END);
}

void CPlayer_Healing::Exit_State()
{
    m_fCoolDown = 0.f;
}

CPlayer_Healing* CPlayer_Healing::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Healing* pInstance = new CPlayer_Healing();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Healing");
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CPlayer_Healing::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
