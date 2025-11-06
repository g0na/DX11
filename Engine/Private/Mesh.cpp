#include "Mesh.h"
#include "Model.h"
#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
    : CVIBuffer { Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(MODEL eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix)
{
    strcpy_s(m_szName, pAIMesh->mName.data);

    m_iMaterialIndex = pAIMesh->mMaterialIndex;

    m_iNumVertices = pAIMesh->mNumVertices;
    m_iIndexStride = 4;

    m_iNumIndices = pAIMesh->mNumFaces * 3;
    m_iNumVertexBuffers = 1;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    HRESULT hr = MODEL::NONANIM == eModelType ?
        Ready_For_NonAnimMesh(pAIMesh, PreTransformMatrix) :
        Ready_For_AnimMesh(pAIMesh, pModel);

    if (FAILED(hr))
        return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC       IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iNumIndices];

    m_pIndices = new _uint[m_iNumIndices];

    _uint iNumIndices = {};

    for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
    {
        pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[0];
        m_pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[0];
        iNumIndices++;

        pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[1];
        m_pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[1];
        iNumIndices++;

        pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[2];
        m_pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[2];
        iNumIndices++;
    }
    
    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_Bones(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
    // 각 본에 대해 최종 변환 행렬을 계산
    for (_uint i = 0; i < m_iNumBones; i++)
    {
        // 최종 본 행렬 = OffsetMatrix * 본의 Combined Matrix
        XMStoreFloat4x4(&m_BoneMatrices[i],
            XMLoadFloat4x4(&m_OffsetMatrices[i]) *
            Bones[m_vecBoneIndices[i]]->Get_CombinedTransformationMatrix());
    }

    // 셰이더에 배열로 전달 (최대 512개)
    return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, g_iMaxNumBones);
}

HRESULT CMesh::Ready_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    m_iVertexStride = sizeof(VTXMESH);

    D3D11_BUFFER_DESC   VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

        m_pVertexPositions[i] = pVertices[i].vPosition;

        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

        memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vBinormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix));

        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_For_AnimMesh(const aiMesh* pAIMesh, CModel* pModel)
{
    m_iVertexStride = sizeof(VTXANIMMESH);

    D3D11_BUFFER_DESC   VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        m_pVertexPositions[i] = pVertices[i].vPosition;

        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
        memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
    }

    m_iNumBones = pAIMesh->mNumBones;

    for (size_t i = 0; i < m_iNumBones; i++)
    {
        aiBone* pAIBone = pAIMesh->mBones[i];

        // 모델의 전체 본 배열에서 현재 본의 인덱스를 가져온다.
        _uint iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);
        if (iBoneIndex == -1)
            return E_FAIL;

        m_vecBoneIndices.push_back(iBoneIndex);

        _float4x4 OffsetMatrix = {};
        memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

        m_OffsetMatrices.push_back(OffsetMatrix);

        // i번째 뼈는 몇개의 정점에게 영향을 주는가?
        for (size_t j = 0; j < pAIBone->mNumWeights; j++)
        {
            aiVertexWeight AIWeight = pAIBone->mWeights[j];

            // AIWeight.mVertexId : i 번째 뼈가 영향을 주는 j 번째 정점의 인덱스
            // 정점의 빈 슬롯에 본 인덱스와 가중치를 저장한다 (최대 4개)
            if (pVertices[AIWeight.mVertexId].vBlendWeight.x == 0.f)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.x = (uint32_t)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
            }
            else if (pVertices[AIWeight.mVertexId].vBlendWeight.y == 0.f)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.y = (uint32_t)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
            }
            else if (pVertices[AIWeight.mVertexId].vBlendWeight.z == 0.f)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.z = (uint32_t)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
            }
            else
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.w = (uint32_t)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
            }
        }
    }

    // 메시에 본이 하나도 없는 경우
    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;

        _float4x4       OffsetMatrix;
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

        m_OffsetMatrices.push_back(OffsetMatrix);
        m_vecBoneIndices.push_back(pModel->Get_BoneIndex(m_szName));        // 메시 이름과 같은 본 사용
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, pModel, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
    CMesh* pInstance = new CMesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMesh::Free()
{
    __super::Free();
}
