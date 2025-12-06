#pragma once
#include "Component.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;

public:
	const COLLIDER			Get_ColType() const { return m_eType; }
	class CBounding*		Get_Bounding() const { return m_pBounding; }

public:
	void Update(_fmatrix WorldMatrix);
	_bool Intersect(CCollider* pTargetCollider);
	
private:
	COLLIDER				m_eType = {};
	class CBounding*		m_pBounding = { nullptr };
	_bool					m_isColl = {};

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

#ifdef _DEBUG
public:
	virtual HRESULT Render();

private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

};

NS_END