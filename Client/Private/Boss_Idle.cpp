#include "Boss_Idle.h"
#include "Monster_Boss.h"
#include "GameInstance.h"
#include "Transform.h"

CBoss_Idle::CBoss_Idle()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_Idle::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr)
        return E_FAIL;

    m_pCurAngle = static_cast<CMonster_Boss*>(m_pOwner)->Get_CurAnglePtr();

    return S_OK;
}

void CBoss_Idle::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(0, true);
}

void CBoss_Idle::Update_State(_float fTimeDelta)
{
    m_fCoolDown += fTimeDelta;

    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Boss_Dead"), false) == true)
        m_pStateMachine->Change_State(CMonster_Boss::DEATH);

    // 방향 전환
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

    m_pMonsterTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), *m_pCurAngle);

    if (m_fCoolDown >= 0.8f)
    {
        if (m_pStateMachine->Get_BoolData(TEXT("Boss_Targeting"), false) == true)
            m_pStateMachine->Change_State(CMonster_Boss::WALK);
        else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 6.f)
        {
            if (m_pGameInstance->Random(1.f, 10.f) >= 6.f)
                m_pStateMachine->Change_State(CMonster_Boss::FLYATTACK);
            else
            {
                if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                    m_pStateMachine->Change_State(CMonster_Boss::BACKSTEP);
                else
                    m_pStateMachine->Change_State(CMonster_Boss::ATTACK);
            }
        }
        else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 14.f)
        {
            if (m_pGameInstance->Random(1.f, 10.f) >= 2.f)
                m_pStateMachine->Change_State(CMonster_Boss::ATTACK);
        }
        else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 18.f)
        {
            if (m_pGameInstance->Random(1.f, 10.f) >= 3.f)
                m_pStateMachine->Change_State(CMonster_Boss::DASHATTACK);
        }
    }
}

void CBoss_Idle::Exit_State()
{
    m_fCoolDown = 0.f;
}

CBoss_Idle* CBoss_Idle::Create(CGameObject* pOwner)
{
    CBoss_Idle* pInstance = new CBoss_Idle();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_Idle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Idle::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
