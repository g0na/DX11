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

    return S_OK;
}

void CPlayer::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyHold(DIK_LEFT))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
    }
    if (m_pGameInstance->Get_KeyHold(DIK_RIGHT))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
    }
    if (m_pGameInstance->Get_KeyHold(DIK_DOWN))
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }

    if (m_pGameInstance->Get_KeyHold(DIK_UP))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);

        if (m_iState & IDLE)
            m_iState ^= IDLE;

        m_iState |= RUN;
    }
    else
    {
        if (m_iState & RUN)
            m_iState ^= RUN;

        m_iState |= IDLE;
    }

    __super::Update(fTimeDelta);
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
