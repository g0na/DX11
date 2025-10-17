#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	explicit CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT	Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);

private:
	_uint									m_iNumTextures = {};
	vector<ID3D11ShaderResourceView*>		m_vecTextures;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END