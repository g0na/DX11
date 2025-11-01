#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath, const aiScene* pAIScene)
{
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint		iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		_char		szTextureFilePath[MAX_PATH] = {};
		_char		szDrive[MAX_PATH] = {};
		_char		szDir[MAX_PATH] = {};

		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

		for (_uint j = 0; j < iNumTextures; j++)
		{
			aiString	strTextureFilePath = {};

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTextureFilePath)))
				return E_FAIL;

			ID3D11ShaderResourceView* pSRV = { nullptr };
			HRESULT hr = {};

			// 임베디드 텍스처 체크
			// GetEmbeddedTexture()로 경로에 해당하는 임베디드 텍스처를 찾음
			const aiTexture* pEmbeddedTexture = pAIScene->GetEmbeddedTexture(strTextureFilePath.data);

			// 임베디드 텍스처가 있는 경우
			if (pEmbeddedTexture != nullptr)
			{
				aiTexture* pAITexture = const_cast<aiTexture*>(pEmbeddedTexture);

				// 압축된 텍스처 (mHeight == 0 이라고 함)
				if (pAITexture->mHeight == 0)
				{
					ScratchImage image = {};
					TexMetadata metadata = {};

					// 임베디드 텍스처의 형식을 나타내주는 힌트
					// FBX, GLTF 같은 모델 파일 안에 텍스쳐가가 임베디드 되어 있을 때 LoadFrom~Memory 사용
					if (strcmp(pAITexture->achFormatHint, "dds") == false)
						hr = LoadFromDDSMemory(reinterpret_cast<const _ubyte*>(pAITexture->pcData), pAITexture->mWidth, DDS_FLAGS_NONE, &metadata, image);

					else if (strcmp(pAITexture->achFormatHint, "tga") == false)
						hr = LoadFromTGAMemory(reinterpret_cast<const _ubyte*>(pAITexture->pcData), pAITexture->mWidth, TGA_FLAGS_NONE, &metadata, image);

					else
						hr = LoadFromWICMemory(reinterpret_cast<const _ubyte*>(pAITexture->pcData), pAITexture->mWidth, WIC_FLAGS_NONE, &metadata, image);

					if (SUCCEEDED(hr))
						hr = CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, &pSRV);						
				}
				else
				{
					// 압축되지 않은 텍스처 (ARGB8888의 형식이라고 함.)
					D3D11_TEXTURE2D_DESC textureDesc = {};
					textureDesc.Width = pAITexture->mWidth;
					textureDesc.Height = pAITexture->mHeight;
					textureDesc.MipLevels = 1;	
					textureDesc.ArraySize = 1;
					textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					textureDesc.SampleDesc.Count = 1;
					textureDesc.SampleDesc.Quality = 0;
					textureDesc.Usage = D3D11_USAGE_DEFAULT;
					textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					textureDesc.CPUAccessFlags = 0;

					D3D11_SUBRESOURCE_DATA initData = {};
					initData.pSysMem = pAITexture->pcData;
					initData.SysMemPitch = pAITexture->mWidth * 4;	// ARGB = 4바이트

					ID3D11Texture2D* pTexture = { nullptr };
					hr = m_pDevice->CreateTexture2D(&textureDesc, &initData, &pTexture);

					if (SUCCEEDED(hr))
					{
						D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
						srvDesc.Format = textureDesc.Format;
						srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
						srvDesc.Texture2D.MipLevels = 1;

						hr = m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSRV);
						Safe_Release(pTexture);
					}
				}
			}
			else
			{
				// 임베디드가 아닌 외부 파일 텍스처
				_char		szFileName[MAX_PATH] = {};
				_char		szExt[MAX_PATH] = {};

				strcpy_s(szTextureFilePath, szDrive);
				strcat_s(szTextureFilePath, szDir);

				_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
				strcat_s(szTextureFilePath, szFileName);
				strcat_s(szTextureFilePath, szExt);

				_tchar		szFinalPath[MAX_PATH] = {};
				MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath), szFinalPath, MAX_PATH);

				if (false == strcmp(szExt, ".dds"))
					hr = CreateDDSTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);

				else if (false == strcmp(szExt, ".tga"))
				{
					ScratchImage image = {};
					TexMetadata metadata = {};

					if (FAILED(LoadFromTGAFile(szFinalPath, TGA_FLAGS_NONE, &metadata, image)))
						return E_FAIL;

					hr = CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, &pSRV);
				}

				else
					hr = CreateWICTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);
			}

			if (FAILED(hr))
				return E_FAIL;

			m_vecSRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iTextureIndex)
{
	return pShader->Bind_SRV(pConstantName, m_vecSRVs[eMaterialType][iTextureIndex]);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath, const aiScene* pAIScene)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pAIMaterial, pModelFilePath, pAIScene)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaterial::Free()
{
	__super::Free();

	for (auto& vecSRVs : m_vecSRVs)
	{
		for (auto& pSRV : vecSRVs)
		{
			Safe_Release(pSRV);
		}
		vecSRVs.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
