#include "Boss_Backstep.h"
#include "Transform.h"
#include "Monster_Boss.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

CBoss_Backstep::CBoss_Backstep()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_Backstep::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pMonsterModel = m_pOwner->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterModel == nullptr)
        return E_FAIL;

    return S_OK;
}

void CBoss_Backstep::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(2, false);

    m_fTimeElapsed = 0.f;
}

void CBoss_Backstep::Update_State(_float fTimeDelta)
{
    // »ç¸Á
    if (m_pStateMachine->Get_BoolData(TEXT("Boss_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Boss::DEATH);
        return;
    }

    // »ç¿îµå
    if (m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() >= 0.667f &&
        m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() <= 0.687f)
        m_pGameInstance->PlaySoundW(TEXT("Boss_foot_slice.wav"), CHANNELID::SOUND_MONSTER, 1.f);
    else if (m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() >= 1.5f &&
        m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() <= 1.52f)
        m_pGameInstance->PlaySoundW(TEXT("Boss_foot.wav"), CHANNELID::SOUND_MONSTER, 1.f);

    m_fTimeElapsed += fTimeDelta;

    if (m_fTimeElapsed >= 2.333f)
        m_pStateMachine->Change_State(CMonster_Boss::IDLE);
}

void CBoss_Backstep::Exit_State()
{
    m_fTimeElapsed = 0.f;
}

CBoss_Backstep* CBoss_Backstep::Create(CGameObject* pOwner)
{
    CBoss_Backstep* pInstance = new CBoss_Backstep();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_Backstep");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Backstep::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
