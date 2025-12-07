#include "Bounding_AABB.h"
#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding { pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(const BOUNDING_DESC* pInitialDesc)
{
	const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(pInitialDesc);

	m_pOriginalDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);

    return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix	TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[0] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[0] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{
	DX::Draw(pBatch, *m_pDesc, isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pInitialDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pInitialDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
