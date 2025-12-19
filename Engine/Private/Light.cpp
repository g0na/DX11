#include "Light.h"
#include "GameInstance.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

void CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint iPassIndex = {};

	if (LIGHT::DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return;

		iPassIndex = ENUM_TO_UINT(DEFERRED::DIRECTIONAL);
	}
	else
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return;
		if (FAILED(pShader->Bind_RawValue("g_vLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return;

		iPassIndex = ENUM_TO_UINT(DEFERRED::POINT);
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return;

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_Resource();

	pVIBuffer->Render();
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();
}
