#include "Player_Attack.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Camera_Free.h"

CPlayer_Attack::CPlayer_Attack()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Attack::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    m_pPlayerModel = pBody->Get_Component<CModel>(TEXT("Com_Model"));
    m_pPlayerCamera = static_cast<CCamera_Free*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Camera")));

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerCamera == nullptr ||
        m_pPlayerModel == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();
    m_pPlayerWeapon = static_cast<CWeapon*>(pPlayer->Find_PartObject(TEXT("Part_Weapon")));

    return S_OK;
}

void CPlayer_Attack::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(32, false);

    // 스태미나 세팅
    m_pStateMachine->Set_BoolData(TEXT("Stamina_Recovery"), false);
    static_cast<CPlayer*>(m_pOwner)->Set_Stamina(20.f);

    // 처음 공격 상태로 들어오면 무조건 공격 횟수 증가
    m_iAttackCnt++;
    m_fAttackDelay = 0.f;

    _vector vCameraLook = m_pPlayerCamera->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::LOOK);
    vCameraLook = XMVector3Normalize(XMVectorSetY(vCameraLook, 0.f));
    _vector vCameraRight = m_pPlayerCamera->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::RIGHT);
    vCameraRight = XMVector3Normalize(XMVectorSetY(vCameraRight, 0.f));

    m_vInputDir = XMVectorZero();
    if (m_pGameInstance->Get_KeyHold(DIK_D))
        m_vInputDir += vCameraRight;
    if (m_pGameInstance->Get_KeyHold(DIK_A))
        m_vInputDir -= vCameraRight;
    if (m_pGameInstance->Get_KeyHold(DIK_S))
        m_vInputDir -= vCameraLook;
    if (m_pGameInstance->Get_KeyHold(DIK_W))
        m_vInputDir += vCameraLook;

    if (!XMVector3Equal(m_vInputDir, XMVectorZero()))
    {
        // 회전 관련
        m_vInputDir = XMVector3Normalize(m_vInputDir);
        _float fAngle = atan2f(XMVectorGetX(m_vInputDir), XMVectorGetZ(m_vInputDir));       // 라디안 반환

        *m_pCurAngle = fAngle;
        m_pPlayerTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), *m_pCurAngle);
        m_pPlayerBody->Set_InputDir(m_vInputDir);
    }

}

void CPlayer_Attack::Update_State(_float fTimeDelta)
{
    // 스태미너 받아오기
    m_fPlayerStamina = static_cast<CPlayer*>(m_pOwner)->Get_CurStamina();

    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false))
    {
        m_pStateMachine->Change_State(CPlayer::DAMAGED);
        return;
    }

    m_fAttackDelay += fTimeDelta;

    _uint iCurAnimIndex = m_pPlayerModel->Get_CurAnimIndex();
    switch (iCurAnimIndex)
    {
    case 32:
        // 사운드 재생
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.466f &&
            m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.486f)
            m_pGameInstance->PlaySoundW(TEXT("swing-sword.wav"), CHANNELID::SOUND_WEAPON, 1.f);

        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.535f &&
            m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.665f)
            m_pPlayerWeapon->Set_CollisionEnabled(true);                // 무기 콜라이더 활성화
        else
            m_pPlayerWeapon->Set_CollisionEnabled(false);                // 무기 콜라이더 비활성화
        break;

    case 33:
        // 사운드 재생
        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.4f &&
            m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.42f)
            m_pGameInstance->PlaySoundW(TEXT("swing-sword2.wav"), CHANNELID::SOUND_WEAPON, 1.f);

        if (m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.465f &&
            m_pPlayerModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.565f)
            m_pPlayerWeapon->Set_CollisionEnabled(true);                // 무기 콜라이더 활성화
        else
            m_pPlayerWeapon->Set_CollisionEnabled(false);                // 무기 콜라이더 비활성화
        break;
    }

    if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB) && m_fPlayerStamina > 0.f)
    {
        // 공격 횟수에 따른 구분
        switch (m_iAttackCnt)
        {
        case 0:
            // 최소 0.76초  최대 2.5초
            if (m_fAttackDelay >= 0.6f)
            {
                m_pPlayerBody->Set_Animation(32, false);
                static_cast<CPlayer*>(m_pOwner)->Set_Stamina(20.f);

                m_iAttackCnt++;
                m_fAttackDelay = 0.f;
            }

            if (m_fAttackDelay >= 2.5f)
                m_pStateMachine->Change_State(CPlayer::IDLE);
            break;

        case 1:
            // 최소 0.73초 최대 2.233초
            if (m_fAttackDelay >= 0.6f)
            {
                m_pPlayerBody->Set_Animation(33, false);
                static_cast<CPlayer*>(m_pOwner)->Set_Stamina(20.f);

                m_iAttackCnt = 0;
                m_fAttackDelay = 0.f;
            }

            if (m_fAttackDelay >= 2.233f)
                m_pStateMachine->Change_State(CPlayer::IDLE);
            break;
        }
    }

    if (m_pGameInstance->Get_KeyDown(DIK_SPACE) && m_fAttackDelay >= 1.f && m_fPlayerStamina > 0.f)
    {
        m_pStateMachine->Change_State(CPlayer::ROLL);
    }
    else if (m_fAttackDelay >= 1.5f)
        m_pStateMachine->Change_State(CPlayer::IDLE);
}

void CPlayer_Attack::Exit_State()
{
    m_pPlayerWeapon->Set_CollisionEnabled(false);
    m_iAttackCnt = 0;
    m_fAttackDelay = 0.f;
    m_pStateMachine->Set_BoolData(TEXT("Stamina_Recovery"), true);
}

CPlayer_Attack* CPlayer_Attack::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Attack* pInstance = new CPlayer_Attack();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Attack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Attack::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
