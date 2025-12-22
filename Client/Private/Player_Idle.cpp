#include "Player_Idle.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"
#include "Camera_Free.h"

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
    m_pPlayerCamera = static_cast<CCamera_Free*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Camera")));

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerCamera == nullptr)
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
    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Dead"), false) == true)
        m_pStateMachine->Change_State(CPlayer::DEATH);

    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false))
        m_pStateMachine->Change_State(CPlayer::DAMAGED);

    _vector vCameraLook = m_pPlayerCamera->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::LOOK);
    vCameraLook = XMVector3Normalize(XMVectorSetY(vCameraLook, 0.f));
    _vector vCameraRight = m_pPlayerCamera->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::RIGHT);
    vCameraRight = XMVector3Normalize(XMVectorSetY(vCameraRight, 0.f));

    _vector vInputDir = XMVectorZero();
    if (m_pGameInstance->Get_KeyHold(DIK_D))
        vInputDir += vCameraRight;
    if (m_pGameInstance->Get_KeyHold(DIK_A))
        vInputDir -= vCameraRight;
    if (m_pGameInstance->Get_KeyHold(DIK_S))
        vInputDir -= vCameraLook;
    if (m_pGameInstance->Get_KeyHold(DIK_W))
        vInputDir += vCameraLook;

    // 구르기
    if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
        m_pStateMachine->Change_State(CPlayer::ROLL);
    // 막기
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::RB))
        m_pStateMachine->Change_State(CPlayer::GUARD);
    // 공격
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB))
        m_pStateMachine->Change_State(CPlayer::ATTACK);
    // 회복
    else if (m_pGameInstance->Get_KeyDown(DIK_R))
        m_pStateMachine->Change_State(CPlayer::HEAL_START);
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
