#include "Player_Recoil.h"
#include "Player.h"
#include "Body.h"
#include "Animation.h"
#include "Model.h"
#include "GameInstance.h"

CPlayer_Recoil::CPlayer_Recoil()
{
}

HRESULT CPlayer_Recoil::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    m_pPlayerModel = m_pPlayerBody->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerModel == nullptr)
        return E_FAIL;

    return S_OK;
}

void CPlayer_Recoil::Enter_State()
{
    if (m_pPlayerBody == nullptr)
        return;

    // 사운드 재생
    m_pGameInstance->PlaySoundW(TEXT("Player_Shield.wav"), CHANNELID::SOUND_WEAPON, 1.f);

    // 넉백 유무에 따른 애니메이션 재생
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Guard_Knockback"), false) == true)
    {
        m_pPlayerBody->Set_Animation(3, false);

        // 스태미나 세팅
        m_pStateMachine->Set_BoolData(TEXT("Stamina_Recovery"), false);
        static_cast<CPlayer*>(m_pOwner)->Set_Stamina(30.f);
    }
    else
    {
        m_pPlayerBody->Set_Animation(2, false);

        // 스태미나 세팅
        m_pStateMachine->Set_BoolData(TEXT("Stamina_Recovery"), false);
        static_cast<CPlayer*>(m_pOwner)->Set_Stamina(15.f);
    }

    m_pStateMachine->Set_BoolData(TEXT("Player_Recoil"), false);
}

void CPlayer_Recoil::Update_State(_float fTimeDelta)
{
    _uint iCurAnimIndex = m_pPlayerModel->Get_CurAnimIndex();
    switch (iCurAnimIndex)
    {
    case 2:
        // 반동 애니 재생 중 반동 상태 재진입
        if (m_pStateMachine->Get_BoolData(TEXT("Player_Recoil"), false) == true)
        {
            Enter_State();
            return;
        }
        break;

    case 3:
        // 무적
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.833f)
            m_pStateMachine->Set_BoolData(TEXT("Player_Invincible"), true);

        // 무적 해제 및 상태 종료
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 1.566f)
        {
            m_pStateMachine->Set_BoolData(TEXT("Player_Invincible"), false);
            m_pStateMachine->Change_State(CPlayer::GUARD);
            return;
        }
        break;
    }

    if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::GUARD);
}

void CPlayer_Recoil::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Player_Recoil"), false);
    m_pStateMachine->Set_BoolData(TEXT("Player_Guard_Knockback"), false);
    m_pStateMachine->Set_BoolData(TEXT("Stamina_Recovery"), true);
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
}
