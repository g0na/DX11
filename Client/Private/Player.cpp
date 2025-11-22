#include "Player.h"
#include "GameInstance.h"
#include "Body.h"

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

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    CBody* pBody = static_cast<CBody*>(Find_PartObject(TEXT("Part_Body")));
    pBody->Set_PlayerTransform(m_pTransformCom);

    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{

    _vector vInputDir = XMVectorZero();

    if (m_pGameInstance->Get_KeyHold(DIK_RIGHT))
    {
        vInputDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_LEFT))
    {
        vInputDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_DOWN))
    {
        vInputDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_UP))
    {
        vInputDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
    }

    // 입력 벡터가 영벡터가 아니라면 입력을 받았다는 뜻
    if (!XMVector3Equal(vInputDir, XMVectorZero()))
    {
        if (m_iState & IDLE)
            m_iState ^= IDLE;

        m_iState |= RUN;

        vInputDir = XMVector3Normalize(vInputDir);

        _float fAngle = atan2f(XMVectorGetX(vInputDir), XMVectorGetZ(vInputDir));       // 라디안 반환
        _float fAngleDiff = fAngle - m_fCurAngle;

        while (fAngleDiff > 180.f)
            fAngleDiff -= 360.f;
        while (fAngleDiff < -180.f)
            fAngleDiff += 360.f;

        _float fDeltaAngle = fAngleDiff * fTimeDelta * 60.f;
        if (abs(fDeltaAngle) > abs(fAngleDiff))
            fDeltaAngle = fAngleDiff;

        m_fCurAngle += fDeltaAngle;
        
        m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fCurAngle);
    }
    else
    {
        if (m_iState & RUN)
            m_iState ^= RUN;

        m_iState |= IDLE;
    }

    __super::Update(fTimeDelta);

    _char buf[512];
    sprintf_s(buf, "x: %f, y: %f, z: %f\n",
        XMVectorGetX(m_pTransformCom->Get_State(STATE::POSITION)),
        XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)),
        XMVectorGetZ(m_pTransformCom->Get_State(STATE::POSITION)));
    OutputDebugStringA(buf);

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
    BodyDesc.pParentState = &m_iState;                                  // 자신의 상태 플래그 전달
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
}
