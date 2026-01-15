#include "Player_PickUp.h"
#include "Transform.h"
#include "Player.h"
#include "Body.h"
#include "Model.h"
#include "Animation.h"
#include "Weapon.h"
#include "Shield.h"
#include "GameInstance.h"

CPlayer_PickUp::CPlayer_PickUp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_PickUp::Initialize(CGameObject* pOwner, CBody* pBody)
{
	__super::Initialize(pOwner);

	m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
	m_pPlayerBody = pBody;
	m_pPlayerModel = pBody->Get_Component<CModel>(TEXT("Com_Model"));

	if (m_pStateMachine == nullptr ||
		m_pPlayerBody == nullptr ||
		m_pPlayerModel == nullptr)
		return E_FAIL;

    CPlayer* pPlayer = static_cast<CPlayer*>(pOwner);

	return S_OK;
}

void CPlayer_PickUp::Enter_State()
{
	if (m_pPlayerBody != nullptr)
		m_pPlayerBody->Set_Animation(24, false);
}

void CPlayer_PickUp::Update_State(_float fTimeDelta)
{
	// 사운드 재생
	if (m_pPlayerModel->Get_Animation(24)->Get_CurrentTrackPosition() >= 0.f &&
		m_pPlayerModel->Get_Animation(24)->Get_CurrentTrackPosition() <= 0.02f)
		m_pGameInstance->PlaySoundW(TEXT("ITEMGET.wav"), CHANNELID::SOUND_EFFECT, 1.f);

	m_fTimeElapsed += fTimeDelta;

	if (m_pPlayerBody->Get_IsAnimFinish() == true)
		m_pStateMachine->Change_State(CPlayer::IDLE);
}

void CPlayer_PickUp::Exit_State()
{
	// 문 열기 상태 비활성화
	m_pStateMachine->Set_BoolData(TEXT("Player_PickUp"), false);
	m_fTimeElapsed = 0.f;
}

CPlayer_PickUp* CPlayer_PickUp::Create(CGameObject* pOwner, CBody* pBody)
{
	CPlayer_PickUp* pInstance = new CPlayer_PickUp();

	if (FAILED(pInstance->Initialize(pOwner, pBody)))
	{
		MSG_BOX("Failed to Created : CPlayer_PickUp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CPlayer_PickUp::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
