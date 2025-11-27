#include "Player_Idle.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"

CPlayer_Idle::CPlayer_Idle()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Idle::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;

    if (m_pStateMachine == nullptr || 
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr)
        return E_FAIL;

    return S_OK;
}

void CPlayer_Idle::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(0, true);
}

void CPlayer_Idle::Update_State(_float fTimeDelta)
{
    _vector vInputDir = XMVectorZero();

    if (m_pGameInstance->Get_KeyHold(DIK_RIGHT))
    {
        vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_LEFT))
    {
        vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_DOWN))
    {
        vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_UP))
    {
        vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    if (!XMVector3Equal(vInputDir, XMVectorZero()))
    {
        m_pStateMachine->Change_State(CPlayer::WALK);
    }

    //if (!XMVector3Equal(vInputDir, XMVectorZero()))
    //{
    //    // 회전 관련
    //    vInputDir = XMVector3Normalize(vInputDir);
    //    _float fAngle = atan2f(XMVectorGetX(vInputDir), XMVectorGetZ(vInputDir));       // 라디안 반환
    //    _float fAngleDiff = fAngle - m_fCurAngle;

    //    while (fAngleDiff > XM_PI)
    //        fAngleDiff -= XM_2PI;
    //    while (fAngleDiff < -XM_PI)
    //        fAngleDiff += XM_2PI;

    //    _float fDeltaAngle = fAngleDiff * fTimeDelta * 30.f;
    //    if (abs(fDeltaAngle) > abs(fAngleDiff))
    //        fDeltaAngle = fAngleDiff;

    //    m_fCurAngle += fDeltaAngle;

    //    m_pPlayerTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fCurAngle);

    //    m_pStateMachine->Change_State(CPlayer::WALK);
    //}
}

void CPlayer_Idle::Exit_State()
{
}

CPlayer_Idle* CPlayer_Idle::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Idle* pInstance = new CPlayer_Idle();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Idle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Idle::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
