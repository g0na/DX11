#include "Boss_Death.h"
#include "Monster_Boss.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

CBoss_Death::CBoss_Death()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_Death::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterModel = m_pOwner->Get_Component<CModel>(TEXT("Com_Model"));
    if (m_pMonsterModel == nullptr)
        return E_FAIL;

    return S_OK;
}

void CBoss_Death::Enter_State()
{

    m_pGameInstance->PlaySoundW(TEXT("Player_Kill.wav"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);

    static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(3, false);
}

void CBoss_Death::Update_State(_float fTimeDelta)
{
    m_fDeathTime += fTimeDelta;

    // »ç¿îµå
    if (m_pMonsterModel->Get_Animation(3)->Get_CurrentTrackPosition() >= 0.234f &&
        m_pMonsterModel->Get_Animation(3)->Get_CurrentTrackPosition() <= 0.254f)
    {
        m_pGameInstance->PlaySoundW(TEXT("Boss_death.wav"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);
        m_pGameInstance->PlaySoundW(TEXT("Boss_Clear.wav"), CHANNELID::SOUND_PARTICLE, 1.f);
    }
    else if (m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() >= 2.766f &&
             m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() <= 2.786f)
             m_pGameInstance->PlaySoundW(TEXT("Boss_foot.wav"), CHANNELID::SOUND_MONSTER, 1.f);
    else if (m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() >= 5.5f &&
             m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() <= 5.52f)
             m_pGameInstance->PlaySoundW(TEXT("Boss_foot.wav"), CHANNELID::SOUND_MONSTER, 1.f);

    if (m_fDeathTime >= 5.5f)
    {
        static_cast<CMonster_Boss*>(m_pOwner)->Set_CollisionEnabled(false);
        m_pGameInstance->Show_UI(TEXT("Prototype_UI_Victory_Back"));
        m_pGameInstance->Show_UI(TEXT("Prototype_UI_Victory"));
    }
    //else if (m_fDeathTime >= 7.333f)
    //    static_cast<CMonster_Boss*>(m_pOwner)->Set_Activity(false);
}

void CBoss_Death::Exit_State()
{
}

CBoss_Death* CBoss_Death::Create(CGameObject* pOwner)
{
    CBoss_Death* pInstance = new CBoss_Death();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Death::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
