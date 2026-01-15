#include "Blood_Boss.h"
#include "GameInstance.h"

CBlood_Boss::CBlood_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CBlood_Boss::CBlood_Boss(const CBlood_Boss& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CBlood_Boss::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBlood_Boss::Initialize(void* pArg)
{   
    lstrcpy(m_szName, TEXT("Effect_Blood_Boss"));

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-11.81f, -14.24f, -45.8f, 1.f));

    m_bIsOn = false;

    return S_OK;
}

void CBlood_Boss::Update_Priority(_float fTimeDelta)
{
}

void CBlood_Boss::Update(_float fTimeDelta)
{
    if (m_bIsOn)
    {
        m_pVIBufferCom->BloodDrop(fTimeDelta);

        if (m_pVIBufferCom->Get_IsFinished())
            m_bIsOn = false;
    }
}

void CBlood_Boss::Update_Late(_float fTimeDelta)
{
    if (!m_bIsOn)
        return;

    m_pGameInstance->Add_RenderObject(RENDERGROUP::NONBLEND, this);
}

HRESULT CBlood_Boss::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;    

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CBlood_Boss::Play(_fvector vResetPosition, _fvector vTargetPosition)
{
    m_bIsOn = true;

    m_pTransformCom->Set_State(STATE::POSITION, vResetPosition);
    
    _vector vDir = vTargetPosition - vResetPosition;
    vDir = XMVector3Normalize(vDir);

    _float3 vFinalDir = {};
    XMStoreFloat3(&vFinalDir, vDir);
    m_pVIBufferCom->Reset(vFinalDir);
}

HRESULT CBlood_Boss::Ready_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Blood_Boss"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxPos_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVELID::GAMEPLAY), TEXT("Prototype_Component_Texture_Blood"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBlood_Boss::Bind_ShaderResources()
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


    return S_OK;
}

CBlood_Boss* CBlood_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBlood_Boss* pInstance = new CBlood_Boss(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBlood_Boss");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBlood_Boss::Clone(void* pArg)
{
    CBlood_Boss* pInstance = new CBlood_Boss(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBlood_Boss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBlood_Boss::Free() 
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
