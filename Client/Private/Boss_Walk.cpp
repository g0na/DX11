#include "Boss_Walk.h"
#include "Transform.h"
#include "Monster_Boss.h"
#include "GameInstance.h"

CBoss_Walk::CBoss_Walk()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_Walk::Initialize(CGameObject* pOwner)
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

void CBoss_Walk::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(1, true);
}

void CBoss_Walk::Update_State(_float fTimeDelta)
{
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

    if (m_pStateMachine->Get_BoolData(TEXT("Boss_Targeting"), false) == false)
        m_pStateMachine->Change_State(CMonster_Boss::IDLE);
    else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 6.f)
    {
        if (m_pGameInstance->Random(1.f, 10.f) >= 6.f)
            m_pStateMachine->Change_State(CMonster_Boss::FLYATTACK);
        else
            m_pStateMachine->Change_State(CMonster_Boss::BACKSTEP);
    }
	else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 14.f)
		m_pStateMachine->Change_State(CMonster_Boss::ATTACK);
	else if (m_pStateMachine->Get_FloatData(TEXT("Boss_Distance"), 999.f) <= 18.f)
		m_pStateMachine->Change_State(CMonster_Boss::DASHATTACK);
}

void CBoss_Walk::Exit_State()
{
}

CBoss_Walk* CBoss_Walk::Create(CGameObject* pOwner)
{
    CBoss_Walk* pInstance = new CBoss_Walk();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_Walk");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Walk::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
