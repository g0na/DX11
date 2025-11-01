#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath, const aiScene* pAIScene);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iTextureIndex);

private:
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*>		m_vecSRVs[AI_TEXTURE_TYPE_MAX];

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath, const aiScene* pAIScene);
	virtual void Free() override;
};

NS_END