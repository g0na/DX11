#include "Player_OpenDoor.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "Model.h"
#include "Weapon.h"
#include "Shield.h"
#include "GameInstance.h"

CPlayer_OpenDoor::CPlayer_OpenDoor()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_OpenDoor::Initialize(CGameObject* pOwner, CBody* pBody)
{
	__super::Initialize(pOwner);

	m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
	m_pPlayerBody = pBody;
	m_pWeapon = static_cast<CWeapon*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Weapon")));
	m_pShield = static_cast<CShield*>(static_cast<CContainerObject*>(m_pOwner)->Find_PartObject(TEXT("Part_Shield")));

	if (m_pStateMachine == nullptr ||
		m_pPlayerBody == nullptr)
		return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);

	return S_OK;
}

void CPlayer_OpenDoor::Enter_State()
{
	if (m_pPlayerBody != nullptr)
		m_pPlayerBody->Set_Animation(22, false);

	// 무기, 방패 비활성화
	m_pWeapon->Set_Activity(false);
	m_pShield->Set_Activity(false);
}

void CPlayer_OpenDoor::Update_State(_float fTimeDelta)
{
	m_fTimeElapsed += fTimeDelta;

	if (m_fTimeElapsed >= 6.9f)
	{
		m_pWeapon->Set_Activity(true);
		m_pShield->Set_Activity(true);
	}

	if (m_pPlayerBody->Get_IsAnimFinish() == true)
		m_pStateMachine->Change_State(CPlayer::IDLE);
}

void CPlayer_OpenDoor::Exit_State()
{
	// 문 열기 상태 비활성화
	m_pStateMachine->Set_BoolData(TEXT("Player_Door"), false);
	m_fTimeElapsed = 0.f;
}

CPlayer_OpenDoor* CPlayer_OpenDoor::Create(CGameObject* pOwner, CBody* pBody)
{
	CPlayer_OpenDoor* pInstance = new CPlayer_OpenDoor();

	if (FAILED(pInstance->Initialize(pOwner, pBody)))
	{
		MSG_BOX("Failed to Created : CPlayer_OpenDoor");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CPlayer_OpenDoor::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
