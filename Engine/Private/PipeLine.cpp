#include "PipeLine.h"
#include "Shader.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Bind_PipeLineMatrix(CShader* pShader, const _char* pConstantName, D3DTS eTransformMatrix)
{
    return pShader->Bind_Matrix(pConstantName, &m_TransformMatrices[ENUM_TO_UINT(eTransformMatrix)]);
}

HRESULT CPipeLine::Bind_PipeLineInverseMatrix(CShader* pShader, const _char* pConstantName, D3DTS eTransformMatrix)
{
    return pShader->Bind_Matrix(pConstantName, &m_TransformInverseMatrices[ENUM_TO_UINT(eTransformMatrix)]);
}

HRESULT CPipeLine::Bind_CamPosition(CShader* pShader, const _char* pConstantName)
{
    return S_OK;
}

void CPipeLine::Update()
{
    for (size_t i = 0; i < ENUM_TO_UINT(D3DTS::END); i++)
    {
        XMStoreFloat4x4(&m_TransformInverseMatrices[i],
            XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i])));
    }

    memcpy(&m_vCamPosition,
        &m_TransformInverseMatrices[ENUM_TO_UINT(D3DTS::VIEW)].m[3], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
    return new CPipeLine();
}

void CPipeLine::Free()
{
    __super::Free();

}
