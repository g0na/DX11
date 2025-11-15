#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	typedef struct tagTextureInfo
	{
		_bool isEmbedded = { false };
		_bool isCompressed = { false };
		_uint iWidth = {};
		_uint iHeight = {};
		_uint iDataSize = {};
		_char szFormatHint[10] = {};
		_ubyte* pData = { nullptr };
		_tchar szFilePath[MAX_PATH] = {};
	}TEXTURE_INFO;

private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath, const aiScene* pAIScene);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iTextureIndex);

public:
	HRESULT Write_To_Binary(ofstream& file);
	HRESULT Read_From_Binary(ifstream& file);

private:
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*>		m_vecSRVs[AI_TEXTURE_TYPE_MAX];
	vector<TEXTURE_INFO>					m_vecTextureInfos[AI_TEXTURE_TYPE_MAX];

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath, const aiScene* pAIScene);
	static CMaterial* Create_Binary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& file);
	virtual void Free() override;
};

NS_END