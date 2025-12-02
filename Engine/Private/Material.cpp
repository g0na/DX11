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

			TEXTURE_INFO textureInfo{};

			ID3D11ShaderResourceView* pSRV = { nullptr };
			HRESULT hr = {};

			// 임베디드 텍스처 체크
			// GetEmbeddedTexture()로 경로에 해당하는 임베디드 텍스처를 찾음
			const aiTexture* pEmbeddedTexture = pAIScene->GetEmbeddedTexture(strTextureFilePath.data);

			// 임베디드 텍스처가 있는 경우
			if (pEmbeddedTexture != nullptr)
			{
				textureInfo.isEmbedded = true;
				textureInfo.iHeight = pEmbeddedTexture->mHeight;
				aiTexture* pAITexture = const_cast<aiTexture*>(pEmbeddedTexture);

				// 압축된 텍스처 (mHeight == 0 이라고 함)
				if (pAITexture->mHeight == 0)
				{
					textureInfo.isCompressed = true;
					textureInfo.iDataSize = pEmbeddedTexture->mWidth;
					strcpy_s(textureInfo.szFormatHint, pEmbeddedTexture->achFormatHint);
					
					textureInfo.pData = new _ubyte[textureInfo.iDataSize];
					memcpy(textureInfo.pData, pEmbeddedTexture->pcData, textureInfo.iDataSize);

					ScratchImage image = {};
					TexMetadata metadata = {};

					// 임베디드 텍스처의 형식을 나타내주는 힌트
					// FBX, GLTF 같은 모델 파일 안에 텍스쳐가 임베디드 되어 있을 때 LoadFrom~Memory 사용
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
					textureInfo.isCompressed = false;
					textureInfo.iWidth = pAITexture->mWidth;
					textureInfo.iHeight = pAITexture->mHeight;
					textureInfo.iDataSize = pAITexture->mWidth * pAITexture->mHeight * 4;

					textureInfo.pData = new _ubyte[textureInfo.iDataSize];
					memcpy(textureInfo.pData, pAITexture->pcData, textureInfo.iDataSize);

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
				textureInfo.isEmbedded = false;

				// 임베디드가 아닌 외부 파일 텍스처
				_char		szFileName[MAX_PATH] = {};
				_char		szExt[MAX_PATH] = {};

				strcpy_s(szTextureFilePath, szDrive);
				strcat_s(szTextureFilePath, szDir);

				_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
				strcat_s(szTextureFilePath, szFileName);
				strcat_s(szTextureFilePath, szExt);

				_tchar		szFinalPath[MAX_PATH] = {};
				MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, (_int)strlen(szTextureFilePath), szFinalPath, MAX_PATH);

				wcscpy_s(textureInfo.szFilePath, szFinalPath);

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

			m_vecTextureInfos[i].push_back(textureInfo);
			m_vecSRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iTextureIndex)
{
	if (m_vecSRVs[eMaterialType].size() == 0)
		eMaterialType = (aiTextureType)6;

	return pShader->Bind_SRV(pConstantName, m_vecSRVs[eMaterialType][iTextureIndex]);
}

HRESULT CMaterial::Write_To_Binary(ofstream& file)
{
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint iNumTextures = (_uint)m_vecSRVs[i].size();
		file.write(reinterpret_cast<_char*>(&iNumTextures), sizeof(_uint));

		for (_uint j = 0; j < iNumTextures; j++)
		{
			file.write(reinterpret_cast<_char*>(&m_vecTextureInfos[i][j].isEmbedded), sizeof(_bool));

			if (m_vecTextureInfos[i][j].isEmbedded)
			{
				// 임베디드 텍스쳐 저장
				// 압축된 텍스처일경우
				_bool isCompressed = (m_vecTextureInfos[i][j].iHeight == 0);
				file.write(reinterpret_cast<_char*>(&isCompressed), sizeof(_bool));

				if (isCompressed)
				{
					// 임베디드 텍스처의 형식을 나타내주는 힌트
					// FBX, GLTF 같은 모델 파일 안에 텍스쳐가 임베디드 되어 있을 때 LoadFrom~Memory 사용
					_uint iFormatLength = (_uint)strlen(m_vecTextureInfos[i][j].szFormatHint);
					file.write(reinterpret_cast<_char*>(&iFormatLength), sizeof(_uint));
					file.write(m_vecTextureInfos[i][j].szFormatHint, sizeof(iFormatLength));

					// 데이타 크기
					file.write(reinterpret_cast<_char*>(&m_vecTextureInfos[i][j].iDataSize), sizeof(_uint));

					// 실제 바이트 데이터
					file.write(reinterpret_cast<_char*>(m_vecTextureInfos[i][j].pData), m_vecTextureInfos[i][j].iDataSize);
				}
				else
				{
					// 압축되지 않은 텍스처 (ARGB8888의 형식이라고 함.)
					file.write(reinterpret_cast<const char*>(&m_vecTextureInfos[i][j].iWidth), sizeof(_uint));
					file.write(reinterpret_cast<const char*>(&m_vecTextureInfos[i][j].iHeight), sizeof(_uint));

					_uint iDataSize = m_vecTextureInfos[i][j].iWidth * m_vecTextureInfos[i][j].iHeight * 4;
					file.write(reinterpret_cast<char*>(&m_vecTextureInfos[i][j].pData), iDataSize);
				}
			}
			else
			{
				// 외부 파일 텍스쳐
				_uint iPathlength = (_uint)wcslen(m_vecTextureInfos[i][j].szFilePath);
				file.write(reinterpret_cast<_char*>(&iPathlength), sizeof(_uint));
				file.write(reinterpret_cast<_char*>(m_vecTextureInfos[i][j].szFilePath), sizeof(_tchar) * iPathlength);
			}
		}
	}

	return S_OK;
}

HRESULT CMaterial::Read_From_Binary(ifstream& file)
{
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint iNumTextures = {};
		file.read(CHARCAST(&iNumTextures), sizeof(_uint));

		for (_uint j = 0; j < iNumTextures; j++)
		{
			_bool isEmbedded = false;
			file.read(CHARCAST(&isEmbedded), sizeof(_bool));

			ID3D11ShaderResourceView* pSRV = nullptr;
			HRESULT hr = {};

			if (isEmbedded)
			{
				_bool isCompressed = false;
				file.read(CHARCAST(&isCompressed), sizeof(_bool));

				if (isCompressed)
				{
					_uint iFormatLength = {};
					file.read(CHARCAST(&iFormatLength), sizeof(_uint));

					_char szFormatHint[10] = {};
					file.read(szFormatHint, sizeof(iFormatLength));
					szFormatHint[iFormatLength] = {};

					_uint iDataSize = {};
					file.read(CHARCAST(&iDataSize), sizeof(_uint));

					_ubyte* pData = new _ubyte[iDataSize];
					file.read(CHARCAST(pData), iDataSize);

					ScratchImage image = {};
					TexMetadata metaData = {};

					if (strcmp(szFormatHint, "dds") == 0)
						hr = LoadFromDDSMemory(pData, iDataSize, DDS_FLAGS_NONE, &metaData, image);
					else if (strcmp(szFormatHint, "tga") == 0)
						hr = LoadFromTGAMemory(pData, iDataSize, TGA_FLAGS_NONE, &metaData, image);
					else
						hr = LoadFromWICMemory(pData, iDataSize, WIC_FLAGS_NONE, &metaData, image);

					if (SUCCEEDED(hr))
						hr = CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metaData, &pSRV);

					Safe_Delete_Array(pData);
				}
				else
				{
					// 압축되지 않은 텍스처 (ARGB8888의 형식이라고 함.)
					_uint iWidth = {}, iHeight = {};
					file.read(CHARCAST(&iWidth), sizeof(_uint));
					file.read(CHARCAST(&iHeight), sizeof(_uint));

					_uint iDataSize = iWidth * iHeight * 4;
					_ubyte* pData = new _ubyte[iDataSize];
					file.read(CHARCAST(pData), iDataSize);

					D3D11_TEXTURE2D_DESC textureDesc = {};

					textureDesc.Width = iWidth;
					textureDesc.Height = iHeight;
					textureDesc.MipLevels = 1;
					textureDesc.ArraySize = 1;
					textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					textureDesc.SampleDesc.Count = 1;
					textureDesc.SampleDesc.Quality = 0;
					textureDesc.Usage = D3D11_USAGE_DEFAULT;
					textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					textureDesc.CPUAccessFlags = 0;

					D3D11_SUBRESOURCE_DATA initData = {};
					initData.pSysMem = pData;
					initData.SysMemPitch = iWidth * 4;	// ARGB = 4바이트

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
					Safe_Delete_Array(pData);
				}
			}
			else
			{
				// 외부 파일 텍스처
				_uint iPathLength = {};
				file.read(CHARCAST(&iPathLength), sizeof(_uint));

				_tchar filePath[MAX_PATH] = {};
				file.read(CHARCAST(filePath), sizeof(_tchar) * iPathLength);
				filePath[iPathLength] = {};

				// 기존 외부 파일 로딩 로직 사용
				_char szExt[MAX_PATH] = {};
				_tchar szExtW[MAX_PATH] = {};

				_wsplitpath_s(filePath, nullptr, 0, nullptr, 0, nullptr, 0, szExtW, MAX_PATH);
				WideCharToMultiByte(CP_ACP, 0, szExtW, -1, szExt, MAX_PATH, nullptr, nullptr);

				if (strcmp(szExt, ".dds") == 0)
					hr = CreateDDSTextureFromFile(m_pDevice, filePath, nullptr, &pSRV);
				else if (strcmp(szExt, ".tga") == 0)
				{
					ScratchImage image = {};
					TexMetadata metadata = {};
					hr = LoadFromTGAFile(filePath, TGA_FLAGS_NONE, &metadata, image);
					if (SUCCEEDED(hr))
						hr = CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, &pSRV);
				}
				else
					hr = CreateWICTextureFromFile(m_pDevice, filePath, nullptr, &pSRV);
			}

			if (SUCCEEDED(hr) && pSRV != nullptr)
				m_vecSRVs[i].push_back(pSRV);
		}
	}
	return S_OK;
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

CMaterial* CMaterial::Create_Binary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& file)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Read_From_Binary(file)))
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

	for (auto& vecTextureInfos : m_vecTextureInfos)
	{
		for (auto& textureInfo : vecTextureInfos)
		{
			Safe_Delete_Array(textureInfo.pData);
		}
		vecTextureInfos.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
