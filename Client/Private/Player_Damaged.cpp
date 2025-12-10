#include "Player_Damaged.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"
#include "Model.h"

CPlayer_Damaged::CPlayer_Damaged()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Damaged::Initialize(CGameObject* pOwner, class CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    Safe_AddRef(m_pPlayerBody);

    if (m_pStateMachine == nullptr ||
        m_pPlayerBody == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();

    return S_OK;
}

void CPlayer_Damaged::Enter_State()
{
    if (m_pOwner != nullptr)
        m_pPlayerBody->Set_Animation(13, false);
}

void CPlayer_Damaged::Update_State(_float fTimeDelta)
{
  /*  if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) == true)
        Enter_State();
    else */if (m_pPlayerBody->Get_IsAnimFinish() == true)
        m_pStateMachine->Change_State(CPlayer::IDLE);
}

void CPlayer_Damaged::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Player_Damaged"), false);
}

CPlayer_Damaged* CPlayer_Damaged::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Damaged* pInstance = new CPlayer_Damaged();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Damaged");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Damaged::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerBody);
    Safe_Release(m_pGameInstance);
}
