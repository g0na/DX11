#include "Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
    CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

    m_pPlayerTransform = pDesc->pPlayerTransform;
    Safe_AddRef(m_pPlayerTransform);
    m_fSensor = pDesc->fSensor;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_fDistance = 4.f;
    m_fYaw = XM_PI;             // 180도
    m_fPitch = 0.5f;

    // 카메라 쉐이킹
    m_fTrauma = 0.f;
    m_fShakeTime = 0.f;
    m_fMaxOffset = 0.5f;
    m_fTraumaDecay = 1.f;

    return S_OK;
}

void CCamera_Free::Update_Priority(_float fTimeDelta)
{
    _long    MouseMove = {};

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
        m_fYaw += (m_fSensor * MouseMove * fTimeDelta);
    if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
        m_fPitch += (m_fSensor * MouseMove * fTimeDelta);

    _float fX = m_fDistance * cos(m_fPitch) * sin(m_fYaw);
    _float fY = m_fDistance * sin(m_fPitch);
    _float fZ = m_fDistance * cos(m_fPitch) * cos(m_fYaw);
    _vector vLocalPosition = XMVectorSet(fX, fY, fZ, 0.f);
    _vector vTargetPosition = m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.5f, 0.f, 0.f);
    vLocalPosition += vTargetPosition;

    m_pTransformCom->Set_State(STATE::POSITION, vLocalPosition);
    m_pTransformCom->LookAt(vTargetPosition);

    if (m_fTrauma > 0.f)
    {
        m_fShakeTime += fTimeDelta;
        
        _float fShakeAmount = m_fTrauma * m_fTrauma;
        _float fOffsetX = (sin(m_fShakeTime * 25.f) + sin(m_fShakeTime * 13.f) * 0.5f) / 1.5f * fShakeAmount * m_fMaxOffset;
        _float fOffsetY = (sin(m_fShakeTime * 20.f) + sin(m_fShakeTime * 10.f) * 0.5f) / 1.5f * fShakeAmount * m_fMaxOffset;
        
        _vector vCurrentPosition = m_pTransformCom->Get_State(STATE::POSITION);
        _vector vRight = m_pTransformCom->Get_State(STATE::RIGHT) * fOffsetX;
        _vector vUp = m_pTransformCom->Get_State(STATE::UP) * fOffsetY;
        vCurrentPosition += vRight + vUp;
        vCurrentPosition = XMVectorSetW(vCurrentPosition, 1.f);
        m_pTransformCom->Set_State(STATE::POSITION, vCurrentPosition);

        m_fTrauma -= m_fTraumaDecay * fTimeDelta;
        if (m_fTrauma <= 0.f)
            m_fTrauma = 0.f;
    }

    // 부모의 월드 행렬을 받아 자신의 최종 변환 행렬을 계산
    __super::SetUp_CombinedWorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
}

void CCamera_Free::Update(_float fTimeDelta)
{
    __super::Update_PipeLine();
}

void CCamera_Free::Update_Late(_float fTimeDelta)
{
    Fix_Camera();
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

void CCamera_Free::Add_Shake(_float fAmount)
{
    m_fTrauma += fAmount;

    if (m_fTrauma >= 1.f)
        m_fTrauma = 1.f;
}

void CCamera_Free::Fix_Camera()
{
    POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    ShowCursor(false);
    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Free");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Free::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerTransform);
}
