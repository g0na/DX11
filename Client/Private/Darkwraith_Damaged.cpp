#include "Darkwraith_Damaged.h"
#include "Transform.h"
#include "Monster_Darkwraith.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

CDarkwraith_Damaged::CDarkwraith_Damaged()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDarkwraith_Damaged::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pMonsterModel = m_pOwner->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterModel == nullptr)
        return E_FAIL;

    m_pCurAngle = static_cast<CMonster_Darkwraith*>(m_pOwner)->Get_CurAnglePtr();

    return S_OK;
}

void CDarkwraith_Damaged::Enter_State()
{
    if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
        m_pGameInstance->PlaySoundW(TEXT("Darkwraith_damage1.wav"), CHANNELID::SOUND_MONSTER, 1.f);
    else
        m_pGameInstance->PlaySoundW(TEXT("Darkwraith_damage2.wav"), CHANNELID::SOUND_MONSTER, 1.f);

    if (m_pOwner != nullptr)
        static_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(2, false);

    m_pStateMachine->Set_BoolData(TEXT("Darkwraith_Damaged"), false);
}

void CDarkwraith_Damaged::Update_State(_float fTimeDelta)
{
    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Darkwraith_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Darkwraith::DEATH);
        return;
    }

    //// 사운드
    //if (m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() >= 0.f &&
    //    m_pMonsterModel->Get_Animation(2)->Get_CurrentTrackPosition() <= 0.02f)
    //{
    //    if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
    //        m_pGameInstance->PlaySoundW(TEXT("blood1.wav"), CHANNELID::SOUND_MONSTER, 1.f);
    //    else
    //        m_pGameInstance->PlaySoundW(TEXT("blood2.wav"), CHANNELID::SOUND_MONSTER, 1.f);

    //    if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
    //        m_pGameInstance->PlaySoundW(TEXT("Hollow_attack.wav"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);
    //    else
    //        m_pGameInstance->PlaySoundW(TEXT("Hollow_attack2.wav"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);
    //}
    
    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Darkwraith_Damaged"), false) == true)
    {
        Enter_State();
        return;
    }
    
    if (m_pOwner->Get_Component<CModel>(TEXT("Com_Model"))->is_AnimFinished() == true)
        m_pStateMachine->Change_State(CMonster_Darkwraith::IDLE);
}

void CDarkwraith_Damaged::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Darkwraith_Damaged"), false);
}

CDarkwraith_Damaged* CDarkwraith_Damaged::Create(CGameObject* pOwner)
{
    CDarkwraith_Damaged* pInstance = new CDarkwraith_Damaged();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CDarkwraith_Damaged");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDarkwraith_Damaged::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
