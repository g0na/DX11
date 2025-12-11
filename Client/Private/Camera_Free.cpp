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

    m_fDistance = 3.f;
    m_fYaw = XM_PI;             // 180도
    m_fPitch = 0.3f;

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
    _vector vLocalPosition = XMVectorSet(fX, fY, fZ, 1.f);

    m_pTransformCom->Set_State(STATE::POSITION, vLocalPosition);
    m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

    // 부모의 월드 행렬을 받아 자신의 최종 변환 행렬을 계산
    __super::SetUp_CombinedWorldMatrix(XMLoadFloat4x4(m_pParentMatrix));

}

void CCamera_Free::Update(_float fTimeDelta)
{
    __super::Update_PipeLine();
}

void CCamera_Free::Update_Late(_float fTimeDelta)
{

}

HRESULT CCamera_Free::Render()
{

    return S_OK;
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
