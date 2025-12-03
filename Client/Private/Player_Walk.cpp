#include "Player_Walk.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "GameInstance.h"

CPlayer_Walk::CPlayer_Walk()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Walk::Initialize(CGameObject* pOwner, CBody* pBody)
{
	__super::Initialize(pOwner);

	m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
	m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
	m_pPlayerBody = pBody;

	if (m_pStateMachine == nullptr ||
		m_pPlayerTransform == nullptr ||
		m_pPlayerBody == nullptr)
		return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);
    m_pCurAngle = pPlayer->Get_CurAnglePtr();

	return S_OK;
}

void CPlayer_Walk::Enter_State()
{
	if (m_pPlayerBody != nullptr)
		m_pPlayerBody->Set_Animation(3, true);
}

void CPlayer_Walk::Update_State(_float fTimeDelta)
{
    // 질주
    if (m_pGameInstance->Get_KeyDown(DIK_LSHIFT))
    {
        m_pStateMachine->Change_State(CPlayer::RUN);
        return;
    }
    // 막기
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::RB))
        m_pStateMachine->Change_State(CPlayer::GUARD);
    // 공격
    else if (m_pGameInstance->Get_MouseBtnDown(MOUSEKEYSTATE::LB))
        m_pStateMachine->Change_State(CPlayer::ATTACK);

    m_vInputDir = XMVectorZero();

    if (m_pGameInstance->Get_KeyHold(DIK_D))
        m_vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);

    if (m_pGameInstance->Get_KeyHold(DIK_A))
        m_vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);

    if (m_pGameInstance->Get_KeyHold(DIK_S))
        m_vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);

    if (m_pGameInstance->Get_KeyHold(DIK_W))
        m_vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);

    if (!XMVector3Equal(m_vInputDir, XMVectorZero()))
    {
        // 회전 관련
        m_vInputDir = XMVector3Normalize(m_vInputDir);
        _float fAngle = atan2f(XMVectorGetX(m_vInputDir), XMVectorGetZ(m_vInputDir));       // 라디안 반환
        _float fAngleDiff = fAngle - *m_pCurAngle;

        while (fAngleDiff > XM_PI)
            fAngleDiff -= XM_2PI;
        while (fAngleDiff < -XM_PI)
            fAngleDiff += XM_2PI;

        _float fDeltaAngle = fAngleDiff * fTimeDelta * 30.f;
        if (abs(fDeltaAngle) > abs(fAngleDiff))
            fDeltaAngle = fAngleDiff;

        *m_pCurAngle += fDeltaAngle;

        m_pPlayerTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), *m_pCurAngle);

        m_pPlayerBody->Set_InputDir(m_vInputDir);

        if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
            m_pStateMachine->Change_State(CPlayer::ROLL);
    }
    else
    {
        m_pStateMachine->Change_State(CPlayer::IDLE);
    }
}

void CPlayer_Walk::Exit_State()
{
}

CPlayer_Walk* CPlayer_Walk::Create(CGameObject* pOwner, CBody* pBody)
{
	CPlayer_Walk* pInstance = new CPlayer_Walk();

	if (FAILED(pInstance->Initialize(pOwner, pBody)))
	{
		MSG_BOX("Failed to Created : CPlayer_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CPlayer_Walk::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
