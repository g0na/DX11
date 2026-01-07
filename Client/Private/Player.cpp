#include "Player.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Body.h"
#include "Weapon.h"
#include "WeaponCase.h"
#include "Shield.h"
#include "Estus.h"
#include "Bounding_Sphere.h"
#include "Collider.h"
#include "Navigation.h"

#include "Player_Idle.h"
#include "Player_Walk.h"
#include "Player_Run.h"
#include "Player_Roll.h"
#include "Player_Guard.h"
#include "Player_Recoil.h"
#include "Player_Attack.h"
#include "Player_Damaged.h"
#include "Player_Death.h"
#include "Player_Ladder.h"
#include "Player_OpenDoor.h"
#include "Player_HealStart.h"
#include "Player_Healing.h"
#include "Player_HealEnd.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject { pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject { Prototype }
{
}

void CPlayer::Set_Damaged(_bool isDamaged, _uint iDamage)
{
    m_pStateMachine->Set_BoolData(TEXT("Player_Damaged"), isDamaged);
    m_fCurHp -= iDamage;

    if (m_fCurHp <= 0.f)
    {
        m_bIsDead = true;
        m_pStateMachine->Set_BoolData(TEXT("Player_Dead"), true);
        return;
    }
}

void CPlayer::Set_Stamina(_float fStamina)
{
    m_fCurStamina -= fStamina;

    if (m_fCurStamina <= 0.f)
        m_fCurStamina = 0.f;
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC    Desc{};
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.f);

    m_eLayer = LAYER::PLAYER;
    m_bIsDead = false;

    m_fMaxHp = 100.f;
    m_fCurHp = m_fMaxHp;
    m_fMaxStamina = 100.f;
    m_fCurStamina = m_fMaxStamina;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    
    if (FAILED(Ready_States()))
        return E_FAIL;

    //x: -11.812585, y : -15.249994, z : -45.795914
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-11.81f, -15.24f, -45.8f, 1.f));

    m_bIsCollisionEnabled = true;

    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    // 오브젝트 상호작용
    if (m_pGameInstance->Get_KeyDown(DIK_E))
    {
        if (m_bLadder)
        {
            m_pStateMachine->Set_BoolData(TEXT("Player_Ladder_Start"), true);

            // 사다리쪽으로 위치 조정
            _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
            vPosition = XMVectorSetX(vPosition, -0.771973f);
            vPosition = XMVectorSetZ(vPosition, 22.446703f);
            m_pTransformCom->Set_State(STATE::POSITION, vPosition);
        }
        else if (m_bDoor)
        {
            m_pStateMachine->Set_BoolData(TEXT("Player_Door"), true);

            // 문 앞으로 위치 조정  x: 3.176330, z: -3.254387
            _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
            vPosition = XMVectorSetX(vPosition, 3.176330f);
            vPosition = XMVectorSetZ(vPosition, -3.254387f);
            m_pTransformCom->Set_State(STATE::POSITION, vPosition);
        }
        else if (m_bOutDoor)
        {
            m_pStateMachine->Set_BoolData(TEXT("Player_Door"), true);

            // 문 앞으로 위치 조정 x: 3.197129, z: -32.945137
            _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
            vPosition = XMVectorSetX(vPosition, 3.197129f);
            vPosition = XMVectorSetZ(vPosition, -32.76f);
            m_pTransformCom->Set_State(STATE::POSITION, vPosition);
        }
    }

    // 루트 모션 적용 이전의 위치 저장
    m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

    // 상태머신 업데이트
    m_pStateMachine->Update_State(fTimeDelta);

    // PartObject들 업데이트
    __super::Update(fTimeDelta);

    // 콜라이더 업데이트
    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CPlayer::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    // 몬스터와 충돌 시 슬라이딩
    if (m_pCollidingObject != nullptr)
    {
        // 루트 모션 이동 벡터
        _vector vMoveDistance = m_pTransformCom->Get_State(STATE::POSITION) - m_vPrevPosition;

        // 충돌 법선 벡터 구하고 정규화
        _vector vCollisionNormal = m_pTransformCom->Get_State(STATE::POSITION) - m_pCollidingObject->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::POSITION);
        _vector vNormal = XMVector3Normalize(vCollisionNormal);

        // 겹친 길이
        _float fPlayerRadius = static_cast<CBounding_Sphere*>(m_pColliderCom->Get_Bounding())->Get_Desc()->Radius;
        _float fMonsterRadius = static_cast<CBounding_Sphere*>(m_pCollidingObject->Get_Component<CCollider>(TEXT("Com_Collider_Sphere"))->Get_Bounding())->Get_Desc()->Radius;

        // 겹친 상태 판단 후 겹친 만큼 플레이어 위치 보정
        _float fCollisionDepth = fPlayerRadius + fMonsterRadius - XMVectorGetX(XMVector3Length(vCollisionNormal));
        if (fCollisionDepth > 0.f)
        {
            // 겹친 만큼 밀어내고 (선형 보간 해야 부드러움)
            _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
            _vector vAfterPosition = vPosition + XMVectorSetW(XMVectorScale(vNormal, fCollisionDepth), 0.f);
            vAfterPosition = XMVectorLerp(vPosition, vAfterPosition, 0.1f);

            // 슬라이딩
            _float fDot = XMVectorGetX(XMVector3Dot(vMoveDistance, vNormal));
            _vector vSliding = vMoveDistance - XMVectorScale(vNormal, fDot);
            vAfterPosition = XMVectorLerp(vAfterPosition, vAfterPosition + XMVectorSetW(vSliding, 0.f), 0.05f);

            vPosition = m_pNavigationCom->Move(m_pTransformCom->Get_State(STATE::POSITION), vAfterPosition);
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(vPosition, 1.f));
        }  
    }

    // 체력, 스태미나 비율 계산
    m_fHpRatio = m_fCurHp / m_fMaxHp;
    
    if (m_pStateMachine->Get_BoolData(TEXT("Stamina_Recovery"), false) == true)
    {
        m_fCurStamina += 33.f * fTimeDelta;

        if (m_fCurStamina >= m_fMaxStamina)
            m_fCurStamina = m_fMaxStamina;
    }

    m_fStaminaRatio = m_fCurStamina / m_fMaxStamina;

    m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);

    // 위치 디버깅
    _char buf[64];
    sprintf_s(buf, "Cell: %d\n", m_pNavigationCom->Get_CurCellIndex());
    OutputDebugStringA(buf);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    if (m_bIsCollisionEnabled == true)
        m_pColliderCom->Render();

    //m_pNavigationCom->Render();
#endif

    return S_OK;
}

void CPlayer::OnCollisionEnter(CGameObject* pOtherObject)
{
    if (pOtherObject->Get_Layer() == TEXT("Layer_Object"))
    {
        if (!lstrcmp(pOtherObject->Get_Name(), TEXT("Ladder")))
        {
            // 사다리에 충돌했을 때 플래그 설정
            m_bLadder = true;
        }
        else if (!lstrcmp(pOtherObject->Get_Name(), TEXT("Door")))
        {
            // 내부문에 충돌했을 때 플래그 설정
            m_bDoor = true;
        }
        else if (!lstrcmp(pOtherObject->Get_Name(), TEXT("OutDoor")))
        {
            // 외부문에 충돌했을 때 플래그 설정
            m_bOutDoor = true;
        }
    }

    if (pOtherObject->Get_Layer() == TEXT("Layer_Monster"))
    {
        m_pCollidingObject = pOtherObject;
    }
    
    if (pOtherObject->Get_Layer() == TEXT("Layer_Weapon"))
    {
        // 무적 상태라면 반환
        if (m_pStateMachine->Get_BoolData(TEXT("Player_Invincible"), false) == true)
            return;

        // 피격 당했는데 가드 중이었다면
        if (m_pStateMachine->Get_BoolData(TEXT("Player_Guard"), false) == true)
            m_pStateMachine->Set_BoolData(TEXT("Player_Recoil"), true);
        else
        {
            m_pStateMachine->Set_BoolData(TEXT("Player_Damaged"), true);
            Set_Damaged(true, 10);
        }
    }

    if (pOtherObject->Get_Layer() == TEXT("Layer_BossWeapon"))
    {
        // 무적 상태라면 반환
        if (m_pStateMachine->Get_BoolData(TEXT("Player_Invincible"), false) == true)
            return;

        // 피격 당했는데 가드 중이었다면
        if (m_pStateMachine->Get_BoolData(TEXT("Player_Guard"), false) == true)
            m_pStateMachine->Set_BoolData(TEXT("Player_Guard_Knockback"), true);
        else
        {
            m_pStateMachine->Set_BoolData(TEXT("Player_Knockback"), true);
            Set_Damaged(true, 40);
        }
    }
}

void CPlayer::OnCollisionExit(CGameObject* pOtherObject)
{
    if (pOtherObject->Get_Layer() == TEXT("Layer_Object"))
    {
        if (!lstrcmp(pOtherObject->Get_Name(), TEXT("Ladder")))
        {
            m_bLadder = false;
            m_pStateMachine->Set_BoolData(TEXT("Player_Ladder_End"), true);
        }
        else if (!lstrcmp(pOtherObject->Get_Name(), TEXT("Door")))
        {
            m_bDoor = false;
            m_pStateMachine->Set_BoolData(TEXT("Player_Door"), false);
        }
        else if (!lstrcmp(pOtherObject->Get_Name(), TEXT("OutDoor")))
        {
            m_bOutDoor = false;
            m_pStateMachine->Set_BoolData(TEXT("Player_Door"), false);
        }
    }

    if (pOtherObject->Get_Layer() == TEXT("Layer_Monster"))
    {
        m_pCollidingObject = nullptr;
    }

    if (pOtherObject->Get_Layer() == TEXT("Layer_Weapon"))
    {
        //Set_Damaged(false, 0);
    }
}

void CPlayer::Heal(_float fHealAmount)
{
    m_fCurHp += fHealAmount;

    if (m_fCurHp >= m_fMaxHp)
        m_fCurHp = m_fMaxHp;
}

HRESULT CPlayer::Ready_Components()
{
    // For Com_StateMachine
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_StateMachine"),
        TEXT("Com_StateMachine"), reinterpret_cast<CComponent**>(&m_pStateMachine))))
        return E_FAIL;

    // For Com_Collider
    CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
    SphereDesc.fRadius = 0.4f;
    SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius + 0.6f, 0.f);

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    // For Com_Navigation
    CNavigation::NAVIGATION_DESC NavigationDesc{};
    NavigationDesc.iCurrentCellIndex = 0;
    NavigationDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_States()
{
    if (FAILED(m_pStateMachine->Add_State(IDLE, CPlayer_Idle::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(WALK, CPlayer_Walk::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(RUN, CPlayer_Run::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(ROLL, CPlayer_Roll::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(GUARD, CPlayer_Guard::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(RECOIL, CPlayer_Recoil::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(ATTACK, CPlayer_Attack::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(DAMAGED, CPlayer_Damaged::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(DEATH, CPlayer_Death::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(LADDER, CPlayer_Ladder::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(DOOR, CPlayer_OpenDoor::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(HEAL_START, CPlayer_HealStart::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(HEAL_ING, CPlayer_Healing::Create(this, m_pBody))))
        return E_FAIL;

    if (FAILED(m_pStateMachine->Add_State(HEAL_END, CPlayer_HealEnd::Create(this, m_pBody))))
        return E_FAIL;

    m_pStateMachine->Set_State(IDLE);

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    // Camera
    CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};
    CameraDesc.vPosition = _float3(0.f, 3.f, -3.f);
    CameraDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    XMStoreFloat3(&CameraDesc.vAt, m_pTransformCom->Get_State(STATE::LOOK));
    CameraDesc.fSpeedPerSec = 0.f;
    CameraDesc.fRotationPerSec = XMConvertToRadians(180.0f);
    CameraDesc.fFovY = XMConvertToRadians(45.0f);
    CameraDesc.fNearZ = 0.1f;
    CameraDesc.fFarZ = 1000.f;
    CameraDesc.fSensor = 0.1f;
    CameraDesc.pPlayerTransform = m_pTransformCom;

    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
        TEXT("Part_Camera"), &CameraDesc)))
        return E_FAIL;

    // Body
    CBody::BODY_DESC    BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();             // 자신의 월드 행렬을 전달
    BodyDesc.fRotationPerSec = 1080.f;

    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
        TEXT("Part_Body"), &BodyDesc)))
        return E_FAIL;
    m_pBody = static_cast<CBody*>(Find_PartObject(TEXT("Part_Body")));    // Body에게 Player의 Transform을 전달
    m_pBody->Set_PlayerTransform(m_pTransformCom);
    m_pBody->Set_PlayerNavigation(m_pNavigationCom);

    // Weapon
    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("R_Weapon");
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Player"),
        TEXT("Part_Weapon"), &WeaponDesc)))
        return E_FAIL;

    // WeaponCase
    CWeaponCase::WEAPONCASE_DESC WeaponCaseDesc{};
    WeaponCaseDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("L_Wepon_Case");
    WeaponCaseDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_WeaponCase_Player"),
        TEXT("Part_WeaponCase"), &WeaponCaseDesc)))
        return E_FAIL;

    // Shield
    CShield::SHIELD_DESC ShieldDesc{};
    ShieldDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("L_Shield");
    ShieldDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Shield_Player"),
        TEXT("Part_Shield"), &ShieldDesc)))
        return E_FAIL;

    // Estus
    CEstus::ESTUS_DESC EstusDesc{};
    EstusDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("R_Hand");
    EstusDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Estus"),
        TEXT("Part_Estus"), &EstusDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_ShaderResources()
{
    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
}
