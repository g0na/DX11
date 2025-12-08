#include "Darkwraith_Attack.h"
#include "Transform.h"
#include "Monster_Darkwraith.h"
#include "GameInstance.h"
#include "Model.h"

CDarkwraith_Attack::CDarkwraith_Attack()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDarkwraith_Attack::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr)
        return E_FAIL;

    m_pCurAngle = dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Get_CurAnglePtr();

    return S_OK;
}

void CDarkwraith_Attack::Enter_State()
{
    if (m_pOwner != nullptr)
        dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(4, false);
}

void CDarkwraith_Attack::Update_State(_float fTimeDelta)
{
    if (m_pStateMachine->Get_BoolData(TEXT("Darkwraith_Damaged"), false) == true)
        m_pStateMachine->Change_State(CMonster_Darkwraith::DAMAGED);

    _vector vPlayerPos = m_pStateMachine->Get_VectorData(TEXT("Player_Position"), XMVectorZero());
    _vector vTargetDir = XMVector3Normalize(XMVectorSetW(vPlayerPos - m_pMonsterTransform->Get_State(STATE::POSITION), 0.f));
    _vector vLook = XMVector3Normalize(m_pMonsterTransform->Get_State(STATE::LOOK));

    m_fAttackDelay += fTimeDelta;

    switch (m_iAttackCnt)
    {
    case 0:
        if (m_fAttackDelay < 0.2f)
        {
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
        }
        else if (m_fAttackDelay >= 1.666f)
        {
            // 거리가 멀어지면 걷기로 변경
            if (m_pStateMachine->Get_FloatData(TEXT("Darkwraith_Distance"), 999.f) > 6.f)
            {
                m_pStateMachine->Change_State(CMonster_Darkwraith::IDLE);
                return;
            }

            dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(4, false);
            m_iAttackCnt++;
            m_fAttackDelay = 0.f;
        }
        break;

    case 1:
        if (m_fAttackDelay < 0.2f)
        {
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
        }
        else if (m_fAttackDelay >= 1.5f)
        {
            // 거리가 멀어지면 걷기로 변경
            if (m_pStateMachine->Get_FloatData(TEXT("Darkwraith_Distance"), 999.f) > 6.f)
            {
                m_pStateMachine->Change_State(CMonster_Darkwraith::IDLE);
                return;
            }

            dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(5, false);
            m_iAttackCnt++;
            m_fAttackDelay = 0.f;
        }
        break;

    case 2:
        if (m_fAttackDelay < 0.2f)
        {
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
        }
        else if (m_fAttackDelay >= 1.166f)
        {
            if (m_pStateMachine->Get_FloatData(TEXT("Darkwraith_Distance"), 999.f) > 6.f)
            {
                m_pStateMachine->Change_State(CMonster_Darkwraith::IDLE);
                return;
            }

            dynamic_cast<CMonster_Darkwraith*>(m_pOwner)->Set_Animation(6, false);
            m_iAttackCnt = 0;
            m_fAttackDelay = 0.f;
        }
        break;
    }
}

void CDarkwraith_Attack::Exit_State()
{
    m_iAttackCnt = 0;
    m_fAttackDelay = 0.f;
}

CDarkwraith_Attack* CDarkwraith_Attack::Create(CGameObject* pOwner)
{
    CDarkwraith_Attack* pInstance = new CDarkwraith_Attack();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CDarkwraith_Attack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDarkwraith_Attack::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
