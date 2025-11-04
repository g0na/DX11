#pragma once
#include "Component.h"

// 정점과 인덱스 버퍼를 만드는 모든 클래스들의 부모 클래스

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
	friend class CCalculator;

protected:
	explicit CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resource();
	virtual HRESULT Render();

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
	_float3*					m_pVertexPositions = { nullptr };
	_uint*						m_pIndices = { nullptr };
	_uint						m_iNumVertices = {};
	_uint						m_iVertexStride = {};
	_uint						m_iNumIndices = {};
	_uint						m_iIndexStride = {};
	_uint						m_iNumVertexBuffers = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveType = {};				// 정점 버퍼를 이용하여 Direct3D에게 기본 도형을 형성하는 방식을 알려주는 수단

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END