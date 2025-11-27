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

	return S_OK;
}

void CPlayer_Walk::Enter_State()
{
	if (m_pPlayerBody != nullptr)
		m_pPlayerBody->Set_Animation(3, true);
}

void CPlayer_Walk::Update_State(_float fTimeDelta)
{
    m_vInputDir = XMVectorZero();

    if (m_pGameInstance->Get_KeyHold(DIK_RIGHT))
    {
        m_vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_LEFT))
    {
        m_vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_DOWN))
    {
        m_vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_UP))
    {
        m_vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    if (!XMVector3Equal(m_vInputDir, XMVectorZero()))
    {
        // 회전 관련
        m_vInputDir = XMVector3Normalize(m_vInputDir);
        _float fAngle = atan2f(XMVectorGetX(m_vInputDir), XMVectorGetZ(m_vInputDir));       // 라디안 반환
        _float fAngleDiff = fAngle - m_fCurAngle;

        while (fAngleDiff > XM_PI)
            fAngleDiff -= XM_2PI;
        while (fAngleDiff < -XM_PI)
            fAngleDiff += XM_2PI;

        _float fDeltaAngle = fAngleDiff * fTimeDelta * 30.f;
        if (abs(fDeltaAngle) > abs(fAngleDiff))
            fDeltaAngle = fAngleDiff;

        m_fCurAngle += fDeltaAngle;

        m_pPlayerTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fCurAngle);

        m_pPlayerBody->Set_InputDir(m_vInputDir);
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
