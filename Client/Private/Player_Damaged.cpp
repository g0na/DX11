#include "Player_Damaged.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

CPlayer_Damaged::CPlayer_Damaged()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Damaged::Initialize(CGameObject* pOwner, class CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    m_pPlayerModel = m_pPlayerBody->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerModel == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();

    return S_OK;
}

void CPlayer_Damaged::Enter_State()
{
    if (m_pOwner == nullptr)
        return;

    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CPlayer::DEATH);
        return;
    }

    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false) == true)
        m_pPlayerBody->Set_Animation(15, false);
    else
        m_pPlayerBody->Set_Animation(14, false);

    m_pStateMachine->Set_BoolData(TEXT("Player_Damaged"), false);
}

void CPlayer_Damaged::Update_State(_float fTimeDelta)
{
    _uint iCurAnimIndex = m_pPlayerModel->Get_CurAnimIndex();
    switch (iCurAnimIndex)
    {
    case 14:        // 피격
        // 사운드 재생
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.f &&
            m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.02f)
        {
            m_pGameInstance->PlaySoundW(TEXT("Player_Damaged.wav"), CHANNELID::SOUND_WEAPON, 1.f);

            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Voice_Damage1.wav"), CHANNELID::SOUND_EFFECT, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Voice_Damage2.wav"), CHANNELID::SOUND_EFFECT, 1.f);
        }

        // 넉백 아닐 때 다시 피격
        if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) == true)
        {
            Enter_State();
            return;
        }
        break;

    case 15:        // 넉백
        // 사운드 재생
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.f &&
            m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.02f)
        {
            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Voice_Damage1.wav"), CHANNELID::SOUND_EFFECT, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Voice_Damage2.wav"), CHANNELID::SOUND_EFFECT, 1.f);
        }
        
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.567f &&
            m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.587f)
            m_pGameInstance->PlaySoundW(TEXT("Player_Roll.wav"), CHANNELID::SOUND_EFFECT, 1.f);

        // 무적
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 3.6f)
            m_pStateMachine->Set_BoolData(TEXT("Player_Invincible"), true);
        // 무적 해제 및 상태 끝
        else if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() > 3.6f)
        {
            m_pStateMachine->Set_BoolData(TEXT("Player_Invincible"), false);
            m_pStateMachine->Change_State(CPlayer::IDLE);
            return;
        }
        break;
    }
        
    if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::IDLE);
}

void CPlayer_Damaged::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Player_Damaged"), false);
    m_pStateMachine->Set_BoolData(TEXT("Player_Knockback"), false);
}

CPlayer_Damaged* CPlayer_Damaged::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Damaged* pInstance = new CPlayer_Damaged();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Damaged");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Damaged::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
