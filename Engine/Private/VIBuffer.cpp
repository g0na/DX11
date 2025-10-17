#include "VIBuffer.h"

USING(Engine)

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	: CComponent{ Prototype }
	, m_pVB { Prototype.m_pVB }
	, m_pIB { Prototype.m_pIB }
	, m_pVertexPositions { Prototype.m_pVertexPositions }
	, m_iNumVertices { Prototype.m_iNumVertices }
	, m_iVertexStride { Prototype.m_iVertexStride }
	, m_iNumIndices { Prototype.m_iNumIndices }
	, m_iIndexStride { Prototype.m_iIndexStride }
	, m_iNumVertexBuffers { Prototype.m_iNumVertexBuffers }
	, m_ePrimitiveType { Prototype.m_ePrimitiveType }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}


HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CVIBuffer::Bind_Resource()
{
	ID3D11Buffer* pVertexBuffers[] =
	{
		m_pVB
	};

	_uint iVertexStrides[] =
	{
		m_iVertexStride
	};

	_uint iOffsets[] =
	{
		0
	};

	// 버퍼를 장치의 한 입력 슬롯으로 묶는 함수
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

	// 기본 도형 형식을 Direct3D에게 알려주는 수단
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	// VTXPOSTEX
	D3D11_INPUT_ELEMENT_DESC		Elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	// 복제품이 아닌 원본일 때만 삭제
	if (m_bIsClone == false)
		Safe_Delete_Array(m_pVertexPositions);

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}