#include "Player_Guard.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"

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

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();

    return S_OK;
}

void CPlayer_Guard::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(1, true);
}

void CPlayer_Guard::Update_State(_float fTimeDelta)
{
    if (m_pGameInstance->Get_MouseBtnUp(MOUSEKEYSTATE::RB))
    {
        m_pStateMachine->Change_State(CPlayer::IDLE);
    }
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB))
        m_pStateMachine->Change_State(CPlayer::ATTACK);
}

void CPlayer_Guard::Exit_State()
{
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
