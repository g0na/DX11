#include "Player_Guard.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"
#include "Camera_Free.h"

CPlayer_Guard::CPlayer_Guard()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Guard::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    m_pPlayerCamera = static_cast<CCamera_Free*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Camera")));

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerCamera == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();

    return S_OK;
}

void CPlayer_Guard::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(1, true);

    m_pStateMachine->Set_BoolData(TEXT("Player_Guard"), true);
}

void CPlayer_Guard::Update_State(_float fTimeDelta)
{
    // 스태미너 받아오기
    m_fPlayerStamina = static_cast<CPlayer*>(m_pOwner)->Get_CurStamina();

    if (m_pStateMachine->Get_BoolData(TEXT("Player_Recoil"), false) == true ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Guard_Knockback"), false) == true)
    {
        m_pStateMachine->Change_State(CPlayer::RECOIL);        
        return;
    }

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

    // 방향키 입력이 있다면
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

        _float fDeltaAngle = fAngleDiff * fTimeDelta * 30.f;
        if (abs(fDeltaAngle) > abs(fAngleDiff))
            fDeltaAngle = fAngleDiff;

        *m_pCurAngle += fDeltaAngle;

        m_pPlayerTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), *m_pCurAngle);

        m_pPlayerBody->Set_InputDir(m_vInputDir);

        if (m_pGameInstance->Get_KeyDown(DIK_SPACE) && m_fPlayerStamina > 0.f)
            m_pStateMachine->Change_State(CPlayer::ROLL);
    }

    if (m_pGameInstance->Get_MouseBtnHold(MOUSEKEYSTATE::RB) == false)
    {
        m_pStateMachine->Change_State(CPlayer::IDLE);
    }
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB) && m_fPlayerStamina > 0.f)
        m_pStateMachine->Change_State(CPlayer::ATTACK);
}

void CPlayer_Guard::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Player_Guard"), false);
}

CPlayer_Guard* CPlayer_Guard::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Guard* pInstance = new CPlayer_Guard();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Guard");
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CPlayer_Guard::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
