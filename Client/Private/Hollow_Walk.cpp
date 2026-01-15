#include "Hollow_Walk.h"
#include "Transform.h"
#include "Monster_Hollow.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

CHollow_Walk::CHollow_Walk()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CHollow_Walk::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pMonsterModel = m_pOwner->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr ||
        m_pMonsterModel == nullptr)
        return E_FAIL;

    m_pCurAngle = static_cast<CMonster_Hollow*>(m_pOwner)->Get_CurAnglePtr();

    return S_OK;
}

void CHollow_Walk::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(1, true);
}

void CHollow_Walk::Update_State(_float fTimeDelta)
{
    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Hollow::DEATH);
        return;
    }

    // 사운드
    if (m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() >= 0.266f &&
        m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() <= 0.286f)
        m_pGameInstance->PlaySoundW(TEXT("Hollow_foot1.wav"), CHANNELID::SOUND_MONSTER, 1.f);
    else if (m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() >= 0.933f &&
             m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() <= 0.953f)
             m_pGameInstance->PlaySoundW(TEXT("Hollow_foot1.wav"), CHANNELID::SOUND_MONSTER, 1.f);
    else if (m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() >= 1.5f &&
             m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() <= 1.52f)
             m_pGameInstance->PlaySoundW(TEXT("Hollow_foot1.wav"), CHANNELID::SOUND_MONSTER, 1.f);
    else if (m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() >= 2.266f &&
             m_pMonsterModel->Get_Animation(1)->Get_CurrentTrackPosition() <= 2.286f)
             m_pGameInstance->PlaySoundW(TEXT("Hollow_foot1.wav"), CHANNELID::SOUND_MONSTER, 1.f);

    m_fCoolDown += fTimeDelta;

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

	if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Damaged"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Hollow::DAMAGED);
        return;
    }

	if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Targeting"), false) == false)
		m_pStateMachine->Change_State(CMonster_Hollow::IDLE);
	else if (m_pStateMachine->Get_FloatData(TEXT("Hollow_Distance"), 999.f) <= 3.f &&
             m_fCoolDown >= 1.f)
		m_pStateMachine->Change_State(CMonster_Hollow::ATTACK); 
}

void CHollow_Walk::Exit_State()
{
    m_fCoolDown = 0.f;
}

CHollow_Walk* CHollow_Walk::Create(CGameObject* pOwner)
{
    CHollow_Walk* pInstance = new CHollow_Walk();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CHollow_Walk");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHollow_Walk::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
