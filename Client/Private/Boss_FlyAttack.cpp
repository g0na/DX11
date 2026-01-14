#include "Boss_FlyAttack.h"
#include "Transform.h"
#include "Monster_Boss.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "FlyCollider_Boss.h"

CBoss_FlyAttack::CBoss_FlyAttack()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_FlyAttack::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pMonsterModel = m_pOwner->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr ||
        m_pMonsterModel == nullptr)
        return E_FAIL;

    m_pBoss = static_cast<CMonster_Boss*>(m_pOwner);
    m_pCurAngle = m_pBoss->Get_CurAnglePtr();
    m_pFlyCollider = static_cast<CFlyCollider_Boss*>(m_pBoss->Find_PartObject(TEXT("Part_FlyCollider_Boss")));

    return S_OK;
}

void CBoss_FlyAttack::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(9, false);
}

void CBoss_FlyAttack::Update_State(_float fTimeDelta)
{
    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Boss_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Boss::DEATH);
        return;
    }

    _vector vPlayerPos = m_pStateMachine->Get_VectorData(TEXT("Player_Position"), XMVectorZero());
    _vector vTargetDir = XMVector3Normalize(XMVectorSetW(vPlayerPos - m_pMonsterTransform->Get_State(STATE::POSITION), 0.f));
    _vector vLook = XMVector3Normalize(m_pMonsterTransform->Get_State(STATE::LOOK));

    m_fFlyAttackDelay += fTimeDelta;

    _uint iCurAnimIndex = m_pMonsterModel->Get_CurAnimIndex();

    // 이펙트, 카메라 플래그
    if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 3.367f &&
        m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 3.6f)
    {
        m_pStateMachine->Set_BoolData(TEXT("Shake_Enable"), true);
        m_bDustFlag = true;
    }

    if (m_bDustFlag)
    {
        m_pBoss->Play_Dust(m_pMonsterTransform->Get_State(STATE::POSITION));
        m_bDustFlag = false;
    }

    if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 3.33f &&
        m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 4.f)
        m_pFlyCollider->Set_CollisionEnabled(true);                 // 콜라이더 활성화
    else
        m_pFlyCollider->Set_CollisionEnabled(false);                // 콜라이더 비활성화

    if (m_fFlyAttackDelay < 0.5f)
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
    else if (m_fFlyAttackDelay >= 6.f)
    {
        m_pStateMachine->Change_State(CMonster_Boss::IDLE);
    }
}

void CBoss_FlyAttack::Exit_State()
{
    m_pFlyCollider->Set_CollisionEnabled(false);
    m_fFlyAttackDelay = 0.f;
}

CBoss_FlyAttack* CBoss_FlyAttack::Create(CGameObject* pOwner)
{
    CBoss_FlyAttack* pInstance = new CBoss_FlyAttack();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_FlyAttack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_FlyAttack::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
