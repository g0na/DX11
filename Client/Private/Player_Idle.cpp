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

    if (m_pGameInstance->Get_KeyHold(DIK_D))
    {
        vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_A))
    {
        vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_S))
    {
        vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_W))
    {
        vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }
    
    // 구르기
    if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
        m_pStateMachine->Change_State(CPlayer::ROLL);
    // 막기
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::RB))
        m_pStateMachine->Change_State(CPlayer::GUARD);
    // 공격
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB))
        m_pStateMachine->Change_State(CPlayer::ATTACK);
    // 걷기
    else if (!XMVector3Equal(vInputDir, XMVectorZero()))
    {
        m_pStateMachine->Change_State(CPlayer::WALK);
    }
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
