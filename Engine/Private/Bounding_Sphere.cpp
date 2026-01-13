#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding { pDevice, pContext }
{
}

HRESULT CBounding_Sphere::Initialize(const BOUNDING_DESC* pInitialDesc)
{
	const BOUNDING_SPHERE_DESC* pDesc = static_cast<const BOUNDING_SPHERE_DESC*>(pInitialDesc);

	m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{
	DX::Draw(pBatch, *m_pDesc, isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}
#endif // _DEBUG

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pInitialDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pInitialDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
