#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& Prototype)
	: CComponent { Prototype }
	, m_iNumTextures { Prototype.m_iNumTextures }
	, m_vecTextures { Prototype.m_vecTextures }
{
	for (auto& pSRV : m_vecTextures)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	_tchar	szDrive[MAX_PATH] = {};
	_tchar	szDir[MAX_PATH] = {};
	_tchar	szEXT[MAX_PATH] = {};

	// 특정 파일 경로를 분할하여 구분해주는 함수
	_wsplitpath_s(pTextureFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, szEXT, MAX_PATH);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };
		_tchar	szFilePath[MAX_PATH] = {};

		wsprintf(szFilePath, pTextureFilePath, i);

		HRESULT hr = {};

		// 파일 확장자명을 비교하여 .tga를 예외처리한다.
		// 나머지 확장자는 각 확장자에 맞는 함수를 호출하여 텍스쳐를 불러온다.
		if (lstrcmp(szEXT, TEXT(".tga")) == false)
		{
			MSG_BOX(".tga file is not supported");
			return E_FAIL;
		}
		else if (lstrcmp(szEXT, TEXT(".dds")) == false)
			hr = CreateDDSTextureFromFile(m_pDevice, szFilePath, nullptr, &pSRV);
		else
			hr = CreateWICTextureFromFile(m_pDevice, szFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		// 불러온 텍스쳐를 컨테이너에 삽입한다.
		m_vecTextures.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_vecTextures[iTextureIndex]);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_vecTextures)
		Safe_Release(pSRV);
	m_vecTextures.clear();

}
