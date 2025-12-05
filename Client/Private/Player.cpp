#include "Player.h"
#include "GameInstance.h"
#include "Body.h"
#include "Weapon.h"
#include "WeaponCase.h"
#include "Shield.h"
#include "Bounding_Sphere.h"
#include "Collider.h"

#include "Player_Idle.h"
#include "Player_Walk.h"
#include "Player_Run.h"
#include "Player_Roll.h"
#include "Player_Guard.h"
#include "Player_Attack.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject { pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject { Prototype }
{
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

    m_bIsDead = false;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    
    if (FAILED(Ready_States()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 5.f, 0.f, 1.f));

    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    // 상태머신 업데이트
    m_pStateMachine->Update_State(fTimeDelta);

    // 콜라이더 업데이트
    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    __super::Update(fTimeDelta);

    // 방향 디버깅
    _char buf[128];
    sprintf_s(buf, "Look x: %f, Look y: %f, Look z: %f\n",
        XMVectorGetX(m_pTransformCom->Get_State(STATE::LOOK)),
        XMVectorGetY(m_pTransformCom->Get_State(STATE::LOOK)),
        XMVectorGetZ(m_pTransformCom->Get_State(STATE::LOOK)));
    OutputDebugStringA(buf);

    // 위치 디버깅
    sprintf_s(buf, "Pos x: %f, Pos y: %f, Pos z:%f\n", 
        XMVectorGetX(m_pTransformCom->Get_State(STATE::POSITION)),
        XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)),
        XMVectorGetZ(m_pTransformCom->Get_State(STATE::POSITION)));
    OutputDebugStringA(buf);
}

void CPlayer::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_StateMachine"),
        TEXT("Com_StateMachine"), reinterpret_cast<CComponent**>(&m_pStateMachine))))
        return E_FAIL;

    // For Com_Collider
    CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
    SphereDesc.fRadius = 0.5f;
    SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
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

    if (FAILED(m_pStateMachine->Add_State(ATTACK, CPlayer_Attack::Create(this, m_pBody))))
        return E_FAIL;

    m_pStateMachine->Set_State(IDLE);

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    // Body
    CBody::BODY_DESC    BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();             // 자신의 월드 행렬을 전달
    BodyDesc.fRotationPerSec = 1080.f;

    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
        TEXT("Part_Body"), &BodyDesc)))
        return E_FAIL;
    m_pBody = static_cast<CBody*>(Find_PartObject(TEXT("Part_Body")));    // Body에게 Player의 Transform을 전달
    m_pBody->Set_PlayerTransform(m_pTransformCom);

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
}
