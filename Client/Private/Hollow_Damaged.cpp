#include "Hollow_Damaged.h"
#include "Transform.h"
#include "Monster_Hollow.h"
#include "GameInstance.h"
#include "Model.h"

CHollow_Damaged::CHollow_Damaged()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CHollow_Damaged::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr)
        return E_FAIL;

    m_pCurAngle = static_cast<CMonster_Hollow*>(m_pOwner)->Get_CurAnglePtr();

    return S_OK;
}

void CHollow_Damaged::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(2, false);

    m_pStateMachine->Set_BoolData(TEXT("Hollow_Damaged"), false);
}

void CHollow_Damaged::Update_State(_float fTimeDelta)
{
    // »ç¸Á
    if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Hollow::DEATH);
        return;
    }
    
    // ÇÇ°Ý
    if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Damaged"), false) == true)
    {
        Enter_State();
        return;
    }
    
    if (m_pOwner->Get_Component<CModel>(TEXT("Com_Model"))->is_AnimFinished() == true)
        m_pStateMachine->Change_State(CMonster_Hollow::IDLE);
}

void CHollow_Damaged::Exit_State()
{
    m_pStateMachine->Set_BoolData(TEXT("Hollow_Damaged"), false);
}

CHollow_Damaged* CHollow_Damaged::Create(CGameObject* pOwner)
{
    CHollow_Damaged* pInstance = new CHollow_Damaged();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CHollow_Damaged");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHollow_Damaged::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
