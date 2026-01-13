#include "Dust.h"
#include "GameInstance.h"

CDust::CDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CDust::CDust(const CDust& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CDust::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDust::Initialize(void* pArg)
{   
    lstrcpy(m_szName, TEXT("Effect_Dust"));

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-11.81f, -14.24f, -45.8f, 1.f));

    m_bIsOn = true;

    m_vScaleUV = _float2(m_fSizeX, m_fSizeY);

    return S_OK;
}

void CDust::Update_Priority(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyDown(DIK_P))
        Play(XMVectorSet(-11.81f, -14.24f, -45.8f, 1.f),
            m_pGameInstance->Get_Player(ENUM_TO_UINT(LEVELID::GAMEPLAY))->Get_Component<CTransform>(g_strTransformTag)->Get_State(STATE::POSITION));
}

void CDust::Update(_float fTimeDelta)
{
    if (m_bIsOn)
    {
        m_fFrameDelay += fTimeDelta;

        if (m_fFrameDelay >= 0.03571f)
        {
            m_vOffsetUV = _float2((m_iCurrentFrameIdx % m_iCountX) * m_fSizeX,
                (m_iCurrentFrameIdx / m_iCountX) * m_fSizeY);

            m_iCurrentFrameIdx++;
            m_fFrameDelay = 0.f;
        }

        if (m_iCurrentFrameIdx >= m_iFrameCnt)
            m_iCurrentFrameIdx = 0;

        m_pVIBufferCom->Spread(fTimeDelta);

        if (m_pVIBufferCom->Get_IsFinished())
            m_bIsOn = false;
    }
}

void CDust::Update_Late(_float fTimeDelta)
{
    if (!m_bIsOn)
        return;

    m_pGameInstance->Add_RenderObject(RENDERGROUP::BLEND, this);
}

HRESULT CDust::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;    

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CDust::Play(_fvector vResetPosition, _fvector vTargetPosition)
{
    m_bIsOn = true;

    m_pTransformCom->Set_State(STATE::POSITION, vResetPosition);

    _vector vDir = vTargetPosition - vResetPosition;
    vDir = XMVector3Normalize(vDir);

    _float3 vFinalDir = {};
    XMStoreFloat3(&vFinalDir, vDir);
    m_pVIBufferCom->Reset(vFinalDir);
}

HRESULT CDust::Ready_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Dust"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxPos_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_Dust"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDust::Bind_ShaderResources()
{  
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
        return E_FAIL;

    // UV 크기, UV Offset 전달하기	g_vOffsetUV, g_vScaleUV
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vOffsetUV", &m_vOffsetUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vScaleUV", &m_vScaleUV, sizeof(_float2))))
        return E_FAIL;

    return S_OK;
}

CDust* CDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDust* pInstance = new CDust(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CDust");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CDust::Clone(void* pArg)
{
    CDust* pInstance = new CDust(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CDust");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDust::Free() 
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
