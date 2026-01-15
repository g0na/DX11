#include "Player_Healing.h"
#include "Player.h"
#include "Body.h"
#include "Estus.h"
#include "UI_Estus.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

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
    m_pPlayerModel = pBody->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pEstus == nullptr ||
        m_pWeapon == nullptr ||
        m_pPlayerModel == nullptr)
        return E_FAIL;

    return S_OK;
}

void CPlayer_Healing::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(26, false);

    // 체력 회복
    static_cast<CPlayer*>(m_pOwner)->Heal(25.f);

    // UI 갱신
    static_cast<CUI_Estus*>(m_pGameInstance->Find_UI(TEXT("Prototype_UI_Estus")))->Set_EstusCount();

    m_fCoolDown = 0.f;
}

void CPlayer_Healing::Update_State(_float fTimeDelta)
{
    // 사운드 재생
    if (m_pPlayerModel->Get_Animation(26)->Get_CurrentTrackPosition() >= 0.f &&
        m_pPlayerModel->Get_Animation(26)->Get_CurrentTrackPosition() <= 0.02f)
        m_pGameInstance->PlaySoundW(TEXT("EST-drink.wav"), CHANNELID::SOUND_EFFECT, 1.f);

    m_fCoolDown += fTimeDelta;

    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CPlayer::DEATH);
        m_pWeapon->Set_Activity(true);
        m_pEstus->Set_Activity(false);

        return;
    }

    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false))
    {
        m_pStateMachine->Change_State(CPlayer::DAMAGED);
        m_pWeapon->Set_Activity(true);
        m_pEstus->Set_Activity(false);

        return;
    }

    // 연속으로 먹기
    if (m_pGameInstance->Get_KeyDown(DIK_R))
    {
        if (m_fCoolDown >= 0.667f)
        {
            Enter_State();
            return;
        }
    }

    // 에스트 먹기
    if (m_pPlayerBody->Get_IsAnimFinish() == true)
    {
        m_pStateMachine->Change_State(CPlayer::HEAL_END);
    }
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
