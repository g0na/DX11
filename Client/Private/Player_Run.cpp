#include "Player_Run.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"

CPlayer_Run::CPlayer_Run()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Run::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();

    return S_OK;
}

void CPlayer_Run::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(12, true);
}

void CPlayer_Run::Update_State(_float fTimeDelta)
{
    m_vInputDir = XMVectorZero();

    if (m_pGameInstance->Get_KeyHold(DIK_D))
    {
        m_vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_A))
    {
        m_vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_S))
    {
        m_vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_W))
    {
        m_vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    // 방향키 입력이 있다면
    if (!XMVector3Equal(m_vInputDir, XMVectorZero()))
    {
        // LSHIFT만 떼면 WALK로 전환
        if (m_pGameInstance->Get_KeyUp(DIK_LSHIFT))
            m_pStateMachine->Change_State(CPlayer::WALK);
        else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::RB))
            m_pStateMachine->Change_State(CPlayer::GUARD);
        
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

        if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
            m_pStateMachine->Change_State(CPlayer::ROLL);
    }
    else
    {
        // 방향키 입력 자체를 안하면 IDLE 상태로 전환
        m_pStateMachine->Change_State(CPlayer::IDLE);
    }
}

void CPlayer_Run::Exit_State()
{
}

CPlayer_Run* CPlayer_Run::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Run* pInstance = new CPlayer_Run();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Run");
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CPlayer_Run::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
