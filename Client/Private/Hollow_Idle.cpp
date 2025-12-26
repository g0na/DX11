#include "Hollow_Idle.h"
#include "Monster_Hollow.h"
#include "GameInstance.h"

CHollow_Idle::CHollow_Idle()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CHollow_Idle::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr)
        return E_FAIL;

    m_pCurAngle = static_cast<CMonster_Hollow*>(m_pOwner)->Get_CurAnglePtr();

    return S_OK;
}

void CHollow_Idle::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(0, true);
}

void CHollow_Idle::Update_State(_float fTimeDelta)
{
    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Hollow::DEATH);
        return;
    }

    m_fCoolDown += fTimeDelta;

    _vector vPlayerPos = m_pStateMachine->Get_VectorData(TEXT("Player_Position"), XMVectorZero());
    _vector vTargetDir = XMVector3Normalize(vPlayerPos - m_pMonsterTransform->Get_State(STATE::POSITION));

    _float fAngle = atan2f(XMVectorGetX(vTargetDir), XMVectorGetZ(vTargetDir));       // 라디안 반환
    _float fAngleDiff = fAngle - *m_pCurAngle;

    while (fAngleDiff > XM_PI)
        fAngleDiff -= XM_2PI;
    while (fAngleDiff < -XM_PI)
        fAngleDiff += XM_2PI;

    _float fDeltaAngle = fAngleDiff * fTimeDelta * 15.f;
    if (abs(fDeltaAngle) > abs(fAngleDiff))
        fDeltaAngle = fAngleDiff;

    *m_pCurAngle += fDeltaAngle;

    if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Targeting"), false))
    {
        if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Damaged"), false))
            m_pStateMachine->Change_State(CMonster_Hollow::DAMAGED);

        if (m_fCoolDown >= 1.5f)
            m_pStateMachine->Change_State(CMonster_Hollow::WALK);
        else
            m_pMonsterTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), *m_pCurAngle);
    } 
}

void CHollow_Idle::Exit_State()
{
    m_fCoolDown = 0.f;
}

CHollow_Idle* CHollow_Idle::Create(CGameObject* pOwner)
{
    CHollow_Idle* pInstance = new CHollow_Idle();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CHollow_Idle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHollow_Idle::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
