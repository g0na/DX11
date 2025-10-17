#include "VIBuffer_Terrain.h"

USING(Engine)

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer { Prototype }
	, m_iNumVerticesX { Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ { Prototype.m_iNumVerticesZ }
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightFileMapPath)
{
	_ulong		dwByte = {};
	HANDLE		hFile = CreateFile(pHeightFileMapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	BITMAPFILEHEADER	fh{};
	BITMAPINFOHEADER	ih{};
	_uint*				pPixels = { nullptr };

	// 높이맵 파일이 맞는지 식별하는 정보가 있음
	if (ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr) == false)
		return E_FAIL;

	// 비트맵 이미지의 너비, 높이, 픽셀 당 비트 수 등 핵심적인 이미지 정보가 있음
	if (ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr) == false)
		return E_FAIL;
	
	// 픽셀의 개수를 읽어들인 파일로부터 동적 생성한다.
	pPixels = new _uint[ih.biWidth * ih.biHeight];
	// 이미지의 실제 픽셀 데이터 전체를 pPixels 배열로 읽어들여 저장한다.
	if (ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr) == false)
		return E_FAIL;

	CloseHandle(hFile);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	// 맵은 버텍스 개수가 약 65000 이상이기 때문에 4로 잡자.
	m_iIndexStride = 4;

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iNumVertexBuffers = 1;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC		VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];	// 저장용 데이터인 _float3
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			// pPixels[iIndex]

			//    11111111 10101111 10101111 10101111

			//&   00000000 00000000 00000000 11111111

			//    00000000 00000000 00000000 10101111
			// 이런 형식이므로 ARGB 중 B 값을 사용하는 것이 편하다.

			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3(j, (pPixels[iIndex] & 0x000000ff) / 15.f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1));
		}
	}

	D3D11_SUBRESOURCE_DATA		VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	// 장치에 버텍스 버퍼를 복사해주고 나면 동적 할당 했던 버텍스 정보를 지운다.
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixels);
#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint iNumIndices = {};

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			_uint	iIndices[4] =
			{
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	D3D11_SUBRESOURCE_DATA		IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightFileMapPath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightFileMapPath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
