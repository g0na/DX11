#include "Player_Idle.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"

CPlayer_Idle::CPlayer_Idle()
{
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

}

void CPlayer_Idle::Update_State(_float fTimeDelta)
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(0, true);
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
}
