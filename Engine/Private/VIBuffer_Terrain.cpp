#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
    : CVIBuffer{ Prototype }
    , m_iNumVerticesX{ Prototype.m_iNumVerticesX }
    , m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightFileMapPath)
{
    _ulong          dwByte = { };
    HANDLE          hFile = CreateFile(pHeightFileMapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    BITMAPFILEHEADER        fh{};
    BITMAPINFOHEADER        ih{};
    _uint* pPixels = { nullptr };

    if (ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr) == false)
        return E_FAIL;
    if (ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr) == false)
        return E_FAIL;

    pPixels = new _uint[ih.biWidth * ih.biHeight];
    if (ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr) == false)
        return E_FAIL;

    CloseHandle(hFile);

    m_iNumVerticesX = ih.biWidth;
    m_iNumVerticesZ = ih.biHeight;

    m_iVertexStride = sizeof(VTXNORTEX);
    m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
    m_iIndexStride = 4;

    m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
    m_iNumVertexBuffers = 1;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    for (_uint i = 0; i < m_iNumVerticesZ; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX; j++)
        {
            _uint       iIndex = i * m_iNumVerticesX + j;

            m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, (pPixels[iIndex] & 0x000000ff) / 15.0f, (_float)i);
            pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
            pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
        }
    }



#pragma endregion

#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iNumIndices];

    _uint       iNumIndices = {};

    for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
        {
            _uint       iIndex = i * m_iNumVerticesX + j;

            _uint       iIndices[4] = {
                iIndex + m_iNumVerticesX,
                iIndex + m_iNumVerticesX + 1,
                iIndex + 1,
                iIndex
            };

            _vector     vSour, vDest, vNormal;

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[1];
            pIndices[iNumIndices++] = iIndices[2];

            vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);

            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
                XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
                XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
                XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);


            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[2];
            pIndices[iNumIndices++] = iIndices[3];


            vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);

            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
                XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
                XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
                XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);

        }
    }

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        XMStoreFloat3(&pVertices[i].vNormal,
            XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
    }

#pragma endregion

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pPixels);

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

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
        MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Terrain::Free()
{
    __super::Free();
}
