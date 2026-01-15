#include "Player_Roll.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Camera_Free.h"

CPlayer_Roll::CPlayer_Roll()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Roll::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    m_pPlayerCamera = static_cast<CCamera_Free*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Camera")));
    m_pPlayerModel = m_pPlayerBody->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerCamera == nullptr ||
        m_pPlayerModel == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();

    return S_OK;
}

void CPlayer_Roll::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(9, false);

    // 스태미나 세팅
    m_pStateMachine->Set_BoolData(TEXT("Stamina_Recovery"), false);
    static_cast<CPlayer*>(m_pOwner)->Set_Stamina(15.f);

    m_CanRoll = false;
    m_fCoolDown = 0.f;

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

void CPlayer_Roll::Update_State(_float fTimeDelta)
{
    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CPlayer::DEATH);
        return;
    }

    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false) == true)
    {
        m_pStateMachine->Change_State(CPlayer::DAMAGED);
        return;
    }

    m_fCoolDown += fTimeDelta;

    // 사운드
    if (m_pPlayerModel->Get_Animation(9)->Get_CurrentTrackPosition() >= 0.2f &&
        m_pPlayerModel->Get_Animation(9)->Get_CurrentTrackPosition() <= 0.22f)
        m_pGameInstance->PlaySoundW(TEXT("Player_Roll.wav"), CHANNELID::SOUND_EFFECT, 1.f);

    // 무적 상태 조정
    if (m_pPlayerModel->Get_Animation(9)->Get_CurrentTrackPosition() <= 0.4f)
        m_pStateMachine->Set_BoolData(TEXT("Player_Invincible"), true);
    else
        m_pStateMachine->Set_BoolData(TEXT("Player_Invincible"), false);

    // 키 입력 가능상황
    if (m_fCoolDown >= 1.1f)
        m_CanRoll = true;

    // 스태미너 받아오기
    m_fPlayerStamina = static_cast<CPlayer*>(m_pOwner)->Get_CurStamina();

    if (m_CanRoll)
    {
        m_vInputDir = XMVectorZero();
        if (m_pGameInstance->Get_KeyHold(DIK_D))
            m_vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
        if (m_pGameInstance->Get_KeyHold(DIK_A))
            m_vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
        if (m_pGameInstance->Get_KeyHold(DIK_S))
            m_vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
        if (m_pGameInstance->Get_KeyHold(DIK_W))
            m_vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);

        if (!XMVector3Equal(m_vInputDir, XMVectorZero()))
        {
            // 회전 관련
            m_vInputDir = XMVector3Normalize(m_vInputDir);
            _float fAngle = atan2f(XMVectorGetX(m_vInputDir), XMVectorGetZ(m_vInputDir));       // 라디안 반환
            _float fAngleDiff = fAngle - *m_pCurAngle;

            while (fAngleDiff > XM_PI)
                fAngleDiff -= XM_2PI;
            while (fAngleDiff < -XM_PI)
                fAngleDiff += XM_2PI;

            _float fDeltaAngle = fAngleDiff * fTimeDelta * 90.f;
            if (abs(fDeltaAngle) > abs(fAngleDiff))
                fDeltaAngle = fAngleDiff;

            *m_pCurAngle += fDeltaAngle;

            m_pPlayerTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), *m_pCurAngle);

            m_pPlayerBody->Set_InputDir(m_vInputDir);

            m_pStateMachine->Change_State(CPlayer::WALK);
            return;
        }

        if (m_pGameInstance->Get_KeyDown(DIK_SPACE) && m_fPlayerStamina > 0.f)
            Enter_State();
    }

    if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::IDLE);
}

void CPlayer_Roll::Exit_State()
{
    m_CanRoll = true;
    m_pStateMachine->Set_BoolData(TEXT("Stamina_Recovery"), true);
}

CPlayer_Roll* CPlayer_Roll::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Roll* pInstance = new CPlayer_Roll();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Roll");
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CPlayer_Roll::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
