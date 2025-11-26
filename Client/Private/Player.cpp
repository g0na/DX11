#include "Player.h"
#include "GameInstance.h"
#include "Body.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject { pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject { Prototype }
    , m_pBody { Prototype.m_pBody }
{
    Safe_AddRef(m_pBody);
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

    m_pBody = static_cast<CBody*>(Find_PartObject(TEXT("Part_Body")));
    m_pBody->Set_PlayerTransform(m_pTransformCom);

    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    _char buf[128];
    sprintf_s(buf, "[Player %p] State: %d\n", this, m_eCurState);
    OutputDebugStringA(buf);

    _vector vInputDir = XMVectorZero();

    if (m_pGameInstance->Get_KeyHold(DIK_RIGHT))
    {
        vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_LEFT))
    {
        vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_DOWN))
    {
        vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_UP))
    {
        vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    m_ePrevState = m_eCurState;

    // 입력 벡터가 영벡터가 아니라면 방향키 입력을 받았다는 뜻
    if (!XMVector3Equal(vInputDir, XMVectorZero()) &&
        m_eCurState != ROLL)
    {
        if (m_pGameInstance->Get_KeyHold(DIK_LSHIFT))
        {
            m_eCurState = RUN;
        }
        else if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
            m_eCurState = ROLL;
        else
            m_eCurState = WALK;

        vInputDir = XMVector3Normalize(vInputDir);
         
        _float fAngle = atan2f(XMVectorGetX(vInputDir), XMVectorGetZ(vInputDir));       // 라디안 반환
        _float fAngleDiff = fAngle - m_fCurAngle;

        while (fAngleDiff > XM_PI)
            fAngleDiff -= XM_2PI;
        while (fAngleDiff < -XM_PI)
            fAngleDiff += XM_2PI;

        _float fDeltaAngle = fAngleDiff * fTimeDelta * 30.f;
        if (abs(fDeltaAngle) > abs(fAngleDiff))
            fDeltaAngle = fAngleDiff;

        m_fCurAngle += fDeltaAngle;
        
        m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fCurAngle);
    }
    else
    {
        if (m_ePrevState != ROLL)
            m_eCurState = IDLE;
        else if (m_ePrevState == ROLL && m_pBody->Get_IsAnimFinish() == true)
            m_eCurState = IDLE;
    }

    __super::Update(fTimeDelta);





    // 위치 디버깅
    sprintf_s(buf, "x: %f, y: %f, z:%f\n", 
        XMVectorGetX(m_pTransformCom->Get_State(STATE::POSITION)),
        XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)),
        XMVectorGetZ(m_pTransformCom->Get_State(STATE::POSITION)));
    OutputDebugStringA(buf);

    // _char buf[512];
    // sprintf_s(buf, "State: %d\n", m_eCurState);
    // OutputDebugStringA(buf);
}

void CPlayer::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

HRESULT CPlayer::Render()
{
    return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    CBody::BODY_DESC    BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();     // 자신의 월드 행렬을 전달
    BodyDesc.pParentState = reinterpret_cast<_uint*>(&m_eCurState);                     // 자신의 상태 전달
    BodyDesc.fRotationPerSec = 1080.f;

    if (FAILED(__super::Add_PartObject(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
        TEXT("Part_Body"), &BodyDesc)))
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

    Safe_Release(m_pBody);
}
