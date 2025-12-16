#include "Darkwraith_Damaged.h"
#include "Transform.h"
#include "Monster_Darkwraith.h"
#include "GameInstance.h"
#include "Model.h"

CDarkwraith_Damaged::CDarkwraith_Damaged()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDarkwraith_Damaged::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr)
        return E_FAIL;

    m_pCurAngle = static_cast<CMonster_Darkwraith*>(m_pOwner)->Get_CurAnglePtr();

    return S_OK;
}

void CDarkwraith_Damaged::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(2, false);
}

void CDarkwraith_Damaged::Update_State(_float fTimeDelta)
{
    if (m_pStateMachine->Get_BoolData(TEXT("Darkwraith_Damaged"), false) == true)
        Enter_State();
    else if (m_pOwner->Get_Component<CModel>(TEXT("Com_Model"))->is_AnimFinished() == true)
        m_pStateMachine->Change_State(CMonster_Darkwraith::IDLE);
}

void CDarkwraith_Damaged::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Darkwraith_Damaged"), false);
}

CDarkwraith_Damaged* CDarkwraith_Damaged::Create(CGameObject* pOwner)
{
    CDarkwraith_Damaged* pInstance = new CDarkwraith_Damaged();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CDarkwraith_Damaged");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDarkwraith_Damaged::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
