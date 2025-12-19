#include "Boss_DashAttack.h"
#include "Transform.h"
#include "Monster_Boss.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Weapon_Boss.h"

CBoss_DashAttack::CBoss_DashAttack()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBoss_DashAttack::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pMonsterModel = m_pOwner->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr ||
        m_pMonsterModel == nullptr)
        return E_FAIL;

    CMonster_Boss* pBoss = static_cast<CMonster_Boss*>(m_pOwner);
    m_pCurAngle = pBoss->Get_CurAnglePtr();
    m_pMonsterWeapon = static_cast<CWeapon_Boss*>(pBoss->Find_PartObject(TEXT("Part_Weapon_Boss")));

    return S_OK;
}

void CBoss_DashAttack::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Boss*>(m_pOwner)->Set_Animation(8, false);
}

void CBoss_DashAttack::Update_State(_float fTimeDelta)
{
    _vector vPlayerPos = m_pStateMachine->Get_VectorData(TEXT("Player_Position"), XMVectorZero());
    _vector vTargetDir = XMVector3Normalize(XMVectorSetW(vPlayerPos - m_pMonsterTransform->Get_State(STATE::POSITION), 0.f));
    _vector vLook = XMVector3Normalize(m_pMonsterTransform->Get_State(STATE::LOOK));

    m_fDashAttackDelay += fTimeDelta;

    _uint iCurAnimIndex = m_pMonsterModel->Get_CurAnimIndex();
    if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 1.868f &&
        m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 2.067f)
        m_pMonsterWeapon->Set_CollisionEnabled(true);                 // 콜라이더 활성화
    else
        m_pMonsterWeapon->Set_CollisionEnabled(false);                // 콜라이더 비활성화

    if (m_fDashAttackDelay < 2.067f)
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
    if (m_fDashAttackDelay >= 4.f)
    {
        m_pStateMachine->Change_State(CMonster_Boss::IDLE);
    }
}

void CBoss_DashAttack::Exit_State()
{
    m_pMonsterWeapon->Set_CollisionEnabled(false);
    m_fDashAttackDelay = 0.f;
}

CBoss_DashAttack* CBoss_DashAttack::Create(CGameObject* pOwner)
{
    CBoss_DashAttack* pInstance = new CBoss_DashAttack();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CBoss_DashAttack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_DashAttack::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
