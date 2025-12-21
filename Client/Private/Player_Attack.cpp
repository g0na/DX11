#include "Player_Attack.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Camera_Free.h"

CPlayer_Attack::CPlayer_Attack()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Attack::Initialize(CGameObject* pOwner, CBody* pBody)
{
    __super::Initialize(pOwner);

    m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pPlayerBody = pBody;
    Safe_AddRef(m_pPlayerBody);
    m_pPlayerCamera = static_cast<CCamera_Free*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Camera")));
    Safe_AddRef(m_pPlayerCamera);

    if (m_pStateMachine == nullptr ||
        m_pPlayerTransform == nullptr ||
        m_pPlayerBody == nullptr ||
        m_pPlayerCamera == nullptr)
        return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();
    m_pPlayerWeapon = static_cast<CWeapon*>(pPlayer->Find_PartObject(TEXT("Part_Weapon")));

    return S_OK;
}

void CPlayer_Attack::Enter_State()
{
    if (m_pPlayerBody != nullptr)
        m_pPlayerBody->Set_Animation(26, false);

    // 처음 공격 상태로 들어오면 무조건 공격 횟수 증가
    m_iAttackCnt++;
    m_fAttackDelay = 0.f;

    _vector vCameraLook = m_pPlayerCamera->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::LOOK);
    vCameraLook = XMVector3Normalize(XMVectorSetY(vCameraLook, 0.f));
    _vector vCameraRight = m_pPlayerCamera->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::RIGHT);
    vCameraRight = XMVector3Normalize(XMVectorSetY(vCameraRight, 0.f));

    m_vInputDir = XMVectorZero();
    if (m_pGameInstance->Get_KeyHold(DIK_D))
        m_vInputDir += vCameraRight;
    if (m_pGameInstance->Get_KeyHold(DIK_A))
        m_vInputDir -= vCameraRight;
    if (m_pGameInstance->Get_KeyHold(DIK_S))
        m_vInputDir -= vCameraLook;
    if (m_pGameInstance->Get_KeyHold(DIK_W))
        m_vInputDir += vCameraLook;

    if (!XMVector3Equal(m_vInputDir, XMVectorZero()))
    {
        // 회전 관련
        m_vInputDir = XMVector3Normalize(m_vInputDir);
        _float fAngle = atan2f(XMVectorGetX(m_vInputDir), XMVectorGetZ(m_vInputDir));       // 라디안 반환

        *m_pCurAngle = fAngle;
        m_pPlayerTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), *m_pCurAngle);
        m_pPlayerBody->Set_InputDir(m_vInputDir);
    }

}

void CPlayer_Attack::Update_State(_float fTimeDelta)
{
    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Player_Damaged"), false) ||
        m_pStateMachine->Get_BoolData(TEXT("Player_Knockback"), false))
    {
        m_pStateMachine->Change_State(CPlayer::DAMAGED);
        return;
    }

    m_fAttackDelay += fTimeDelta;

    if (m_fAttackDelay >= 0.5f && m_fAttackDelay < 0.6f)
        m_pPlayerWeapon->Set_CollisionEnabled(true);                // 무기 콜라이더 활성화
    else
        m_pPlayerWeapon->Set_CollisionEnabled(false);                // 무기 콜라이더 비활성화

    if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB))
    {
        // 공격 횟수에 따른 구분
        switch (m_iAttackCnt)
        {
        case 0:
            // 최소 0.76초  최대 2.5초
            if (m_fAttackDelay >= 0.6f)
            {
                m_pPlayerBody->Set_Animation(26, false);

                m_iAttackCnt++;
                m_fAttackDelay = 0.f;
            }

            if (m_fAttackDelay >= 2.5f)
                m_pStateMachine->Change_State(CPlayer::IDLE);
            break;

        case 1:
            // 최소 0.73초 최대 2.233초
            if (m_fAttackDelay >= 0.6f)
            {
                m_pPlayerBody->Set_Animation(27, false);

                m_iAttackCnt = 0;
                m_fAttackDelay = 0.f;
            }

            if (m_fAttackDelay >= 2.233f)
                m_pStateMachine->Change_State(CPlayer::IDLE);
            break;
        }
    }

    if (m_pGameInstance->Get_KeyDown(DIK_SPACE) && m_fAttackDelay >= 1.f)
    {
        m_pStateMachine->Change_State(CPlayer::ROLL);
    }
    else if (m_fAttackDelay >= 1.5f)
        m_pStateMachine->Change_State(CPlayer::IDLE);
}

void CPlayer_Attack::Exit_State()
{
    m_pPlayerWeapon->Set_CollisionEnabled(false);
    m_iAttackCnt = 0;
    m_fAttackDelay = 0.f;
}

CPlayer_Attack* CPlayer_Attack::Create(CGameObject* pOwner, CBody* pBody)
{
    CPlayer_Attack* pInstance = new CPlayer_Attack();

    if (FAILED(pInstance->Initialize(pOwner, pBody)))
    {
        MSG_BOX("Failed to Created : CPlayer_Attack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Attack::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerBody);
    Safe_Release(m_pPlayerCamera);
    Safe_Release(m_pGameInstance);
}
