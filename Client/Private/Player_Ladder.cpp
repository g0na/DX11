#include "Player_Ladder.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "Model.h"
#include "Weapon.h"
#include "Shield.h"
#include "GameInstance.h"

CPlayer_Ladder::CPlayer_Ladder()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_Ladder::Initialize(CGameObject* pOwner, CBody* pBody)
{
	__super::Initialize(pOwner);

	m_pPlayerTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
	m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
	m_pPlayerBody = pBody;
	m_pPlayerModel = m_pPlayerBody->Get_Component<CModel>(TEXT("Com_Model"));
	m_pWeapon = static_cast<CWeapon*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Weapon")));
	m_pShield = static_cast<CShield*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Shield")));

	if (m_pStateMachine == nullptr ||
		m_pPlayerTransform == nullptr ||
		m_pPlayerBody == nullptr ||
		m_pPlayerModel == nullptr)
		return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);

	return S_OK;
}

void CPlayer_Ladder::Enter_State()
{
	// 사다리 상태 활성화
	m_pPlayerBody->Set_IsOnLadder(true);
	m_pStateMachine->Set_BoolData(TEXT("Player_Ladder_End"), false);

	// 무기, 방패 비활성화
	m_pWeapon->Set_Activity(false);
	m_pShield->Set_Activity(false);

	if (m_pPlayerBody != nullptr)
		m_pPlayerBody->Set_Animation(17, false);
}

void CPlayer_Ladder::Update_State(_float fTimeDelta)
{
    m_vInputDir = XMVectorZero();
	if (m_pGameInstance->Get_KeyHold(DIK_D))
		m_vInputDir += XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (m_pGameInstance->Get_KeyHold(DIK_A))
		m_vInputDir -= XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (m_pGameInstance->Get_KeyHold(DIK_S))
		m_vInputDir -= XMVectorSet(0.f, -1.f, 0.f, 0.f);
	if (m_pGameInstance->Get_KeyHold(DIK_W))
		m_vInputDir += XMVectorSet(0.f, 1.f, 0.f, 0.f);

    if (!XMVector3Equal(m_vInputDir, XMVectorZero()))
    {
		_uint iCurAnimIndex = m_pPlayerModel->Get_CurAnimIndex();
		switch (iCurAnimIndex)
		{
		case 17:
			if (m_pPlayerBody->Get_IsAnimFinish() == true)
				m_pPlayerBody->Set_Animation(18, false);
			break;

		case 18:
			if (m_pPlayerBody->Get_IsAnimFinish() == true)
				m_pPlayerBody->Set_Animation(19, false);
			break;

		case 19:
			if (m_pPlayerBody->Get_IsAnimFinish() == true)
			{
				if (m_pStateMachine->Get_BoolData(TEXT("Player_Ladder_End"), false) == true)
				{
					m_pPlayerBody->Set_Animation(20, false);
				}
				else
					m_pPlayerBody->Set_Animation(18, false);
			}
			break;

		case 20:
			m_fTimeElapsed += fTimeDelta;

			if (m_fTimeElapsed >= 1.5f)
			{
				m_pWeapon->Set_Activity(true);
				m_pShield->Set_Activity(true);
			}

			if (m_pPlayerBody->Get_IsAnimFinish() == true)
			{
				m_pStateMachine->Change_State(CPlayer::IDLE);
				return;
			}
			break;
		}
	}
}

void CPlayer_Ladder::Exit_State()
{
	// 사다리 상태 비활성화
	m_pPlayerBody->Set_IsOnLadder(false);
	m_pStateMachine->Set_BoolData(TEXT("Player_Ladder_Start"), false);
	m_fTimeElapsed = 0.f;
}

CPlayer_Ladder* CPlayer_Ladder::Create(CGameObject* pOwner, CBody* pBody)
{
	CPlayer_Ladder* pInstance = new CPlayer_Ladder();

	if (FAILED(pInstance->Initialize(pOwner, pBody)))
	{
		MSG_BOX("Failed to Created : CPlayer_Ladder");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CPlayer_Ladder::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
