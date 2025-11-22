#include "Body.h"
//#include "Model.h"
#include "Player.h"
#include "GameInstance.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject { pDevice, pContext }
{
}

CBody::CBody(const CBody& Prototype)
    : CPartObject { Prototype }
{
}

HRESULT CBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody::Initialize(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    m_pParentState = pDesc->pParentState;
    m_fRotationSpeed = pDesc->fRotationPerSec;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_Animation(0, true);

    return S_OK;
}

void CBody::Update_Priority(_float fTimeDelta)
{
    // 부모의 월드 행렬을 받아 자신의 최종 변환 행렬을 계산
    __super::SetUp_CombinedWorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
}

void CBody::Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);

    _vector vRootMotionDelta = m_pModelCom->Get_RootMotionDelta();

    _float3	fDelta = {};
    XMStoreFloat3(&fDelta, vRootMotionDelta);

    // 로컬 공간의 루트 모션 델타를 월드 공간의 벡터로 바꿔줘야 내가 바꾼 회전이 적용댐!
    _vector vRight = m_pPlayerTransform->Get_State(STATE::RIGHT);
    _vector vUp = m_pPlayerTransform->Get_State(STATE::UP);
    _vector vLook = m_pPlayerTransform->Get_State(STATE::LOOK);

    m_vWorldDelta = vRight * fDelta.x + vUp * fDelta.y + vLook * fDelta.z;
    _vector vPosition = m_pPlayerTransform->Get_State(STATE::POSITION);
    vPosition += m_vWorldDelta;
    m_pPlayerTransform->Set_State(STATE::POSITION, vPosition);

}

void CBody::Update_Late(_float fTimeDelta)
{
    if (*m_pParentState & CPlayer::IDLE)
        m_pModelCom->Set_Animation(0, true);

    if (*m_pParentState & CPlayer::WALK)
        m_pModelCom->Set_Animation(1, true);

    if (*m_pParentState & CPlayer::RUN)
    {
        m_pModelCom->Set_Animation(2, true);
        
        //_char buf[512];
        //sprintf_s(buf, "Angle : %f\n", m_fCurAngle);
        //OutputDebugStringA(buf);
    }

    m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CBody::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody::Ready_Components()
{
    // For Com_Model
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Model_Darkwraith"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    // For Com_Shader
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMartix)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (pLightDesc == nullptr)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;
    
    return S_OK;
}

CBody* CBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody* pInstance = new CBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody::Clone(void* pArg)
{
    CBody* pInstance = new CBody(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
