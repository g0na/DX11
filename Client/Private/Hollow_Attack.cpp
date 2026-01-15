#include "Hollow_Attack.h"
#include "Transform.h"
#include "Monster_Hollow.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Weapon.h"

CHollow_Attack::CHollow_Attack()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CHollow_Attack::Initialize(CGameObject* pOwner)
{
    __super::Initialize(pOwner);

    m_pMonsterTransform = m_pOwner->Get_Component<CTransform>(g_strTransformTag);
    m_pStateMachine = m_pOwner->Get_Component<CStateMachine>(TEXT("Com_StateMachine"));
    m_pMonsterModel = m_pOwner->Get_Component<CModel>(TEXT("Com_Model"));

    if (m_pStateMachine == nullptr ||
        m_pMonsterTransform == nullptr ||
        m_pMonsterModel == nullptr)
        return E_FAIL;

    CMonster_Hollow* pHollow = static_cast<CMonster_Hollow*>(m_pOwner);
    m_pCurAngle = pHollow->Get_CurAnglePtr();
    m_pMonsterWeapon = static_cast<CWeapon*>(pHollow->Find_PartObject(TEXT("Part_Weapon_Hollow")));

    return S_OK;
}

void CHollow_Attack::Enter_State()
{
    if (m_pOwner != nullptr)
        static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(4, false);
    
    m_iAttackCnt++;
}

void CHollow_Attack::Update_State(_float fTimeDelta)
{
    // 사망
    if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Dead"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Hollow::DEATH);
        return;
    }

    // 피격
    if (m_pStateMachine->Get_BoolData(TEXT("Hollow_Damaged"), false) == true)
    {
        m_pStateMachine->Change_State(CMonster_Hollow::DAMAGED);
        return;
    }

    _vector vPlayerPos = m_pStateMachine->Get_VectorData(TEXT("Player_Position"), XMVectorZero());
    _vector vTargetDir = XMVector3Normalize(XMVectorSetW(vPlayerPos - m_pMonsterTransform->Get_State(STATE::POSITION), 0.f));
    _vector vLook = XMVector3Normalize(m_pMonsterTransform->Get_State(STATE::LOOK));

    m_fAttackDelay += fTimeDelta;

    _uint iCurAnimIndex = m_pMonsterModel->Get_CurAnimIndex();
    switch (iCurAnimIndex)
    {
    case 4:
        // 사운드
        if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.866f &&
            m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.886f)
        {
            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Hollow_swing.mp3"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Hollow_swing2.mp3"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);

            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Hollow_attack3.wav"), CHANNELID::SOUND_MONSTER, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Hollow_attack4.mp3"), CHANNELID::SOUND_MONSTER, 1.f);
        }

        if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.96f &&
            m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 1.1f)
            m_pMonsterWeapon->Set_CollisionEnabled(true);                // 무기 콜라이더 활성화
        else
            m_pMonsterWeapon->Set_CollisionEnabled(false);                // 무기 콜라이더 비활성화
        break;

    case 5:
        // 사운드
        if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.633f &&
            m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.653f)
        {
            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Hollow_swing.mp3"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Hollow_swing2.mp3"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);

            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Hollow_attack3.wav"), CHANNELID::SOUND_MONSTER, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Hollow_attack4.mp3"), CHANNELID::SOUND_MONSTER, 1.f);
        }

        if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.7f &&
            m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.832f)
            m_pMonsterWeapon->Set_CollisionEnabled(true);                // 무기 콜라이더 활성화
        else
            m_pMonsterWeapon->Set_CollisionEnabled(false);                // 무기 콜라이더 비활성화
        break;

    case 6:
        // 사운드
        if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.833f &&
            m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 0.853f)
        {
            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Hollow_swing.mp3"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Hollow_swing2.mp3"), CHANNELID::SOUND_MONSTER_WEAPON, 1.f);

            if (m_pGameInstance->Random(1.f, 10.f) >= 5.f)
                m_pGameInstance->PlaySoundW(TEXT("Hollow_attack3.wav"), CHANNELID::SOUND_MONSTER, 1.f);
            else
                m_pGameInstance->PlaySoundW(TEXT("Hollow_attack4.mp3"), CHANNELID::SOUND_MONSTER, 1.f);
        }   

        if (m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() >= 0.967f &&
            m_pMonsterModel->Get_Animation(iCurAnimIndex)->Get_CurrentTrackPosition() <= 1.166f)
            m_pMonsterWeapon->Set_CollisionEnabled(true);                // 무기 콜라이더 활성화
        else
            m_pMonsterWeapon->Set_CollisionEnabled(false);                // 무기 콜라이더 비활성화
        break;

    default:
        m_pMonsterWeapon->Set_CollisionEnabled(false);                // 무기 콜라이더 비활성화
        break;
    }

    switch (m_iAttackCnt)
    {
    case 0:
        if (m_fAttackDelay < 0.2f)
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
        else if (m_fAttackDelay >= 2.567f)
        {
            // 거리가 멀어지면 걷기로 변경
            if (m_pStateMachine->Get_FloatData(TEXT("Hollow_Distance"), 999.f) > 6.f)
            {
                m_pStateMachine->Change_State(CMonster_Hollow::IDLE);
                return;
            }

            static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(4, false);
            m_iAttackCnt++;
            m_fAttackDelay = 0.f;
        }
        break;

	case 1:
		if (m_fAttackDelay < 0.2f)
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
		else if (m_fAttackDelay >= 1.333f)
		{
			// 거리가 멀어지면 걷기로 변경
			if (m_pStateMachine->Get_FloatData(TEXT("Hollow_Distance"), 999.f) > 6.f)
			{
				m_pStateMachine->Change_State(CMonster_Hollow::IDLE);
				return;
			}

            static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(5, false);
			m_iAttackCnt++;
			m_fAttackDelay = 0.f;
		}
		break;

    case 2:
        if (m_fAttackDelay < 0.2f)
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
        else if (m_fAttackDelay >= 1.333f)
        {
            // 거리가 멀어지면 걷기로 변경
            if (m_pStateMachine->Get_FloatData(TEXT("Hollow_Distance"), 999.f) > 6.f)
            {
                m_pStateMachine->Change_State(CMonster_Hollow::IDLE);
                return;
            }

            static_cast<CMonster_Hollow*>(m_pOwner)->Set_Animation(6, false);
            m_iAttackCnt = 0;
            m_fAttackDelay = 0.f;
        }
        break;
    }
}

void CHollow_Attack::Exit_State()
{
    m_pMonsterWeapon->Set_CollisionEnabled(false);
    m_iAttackCnt = 0;
    m_fAttackDelay = 0.f;
}

CHollow_Attack* CHollow_Attack::Create(CGameObject* pOwner)
{
    CHollow_Attack* pInstance = new CHollow_Attack();

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("Failed to Created : CHollow_Attack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHollow_Attack::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
