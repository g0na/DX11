#include "Calculator.h"
#include "Transform.h"
#include "Mesh.h"
#include "PipeLine.h"
#include "GameInstance.h"

CCalculator::CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
	
}

HRESULT CCalculator::Initialize_Prototype()
{
	//m_pPipeLine = CPipeLine::Create();
	//if (m_pPipeLine == nullptr)
	//	return E_FAIL;

	//Safe_AddRef(m_pPipeLine);

	return S_OK;
}

HRESULT CCalculator::Initialize(void* pArg)
{
	return S_OK;
}

//_float CCalculator::Compute_HeightOnTerrain(const _vector* pPos,
//	const _vector* pTerrainVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
//{
//	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);
//
//	_float	fWidth  = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
//	_float	fHeight = ( pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;
//
//	D3DXPLANE		Plane;
//
//	// �� ��� 
//	if (fWidth > fHeight)
//	{
//		D3DXPlaneFromPoints(&Plane, 
//			&pTerrainVtxPos[dwIndex + dwCntX],
//			&pTerrainVtxPos[dwIndex + dwCntX + 1],
//			&pTerrainVtxPos[dwIndex + 1]);
//	}
//	// �� �ϴ�
//	else
//	{
//		D3DXPlaneFromPoints(&Plane,
//			&pTerrainVtxPos[dwIndex + dwCntX],
//			&pTerrainVtxPos[dwIndex + 1],
//			&pTerrainVtxPos[dwIndex]);
//	}
//
//	// ax + by + cz + d = 0
//
//	// by = -ax - cz - d
//	// 
//	// y = (-ax - cz - d) / b
//	
//	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
//}
//

_vector CCalculator::Picking_OnMesh(HWND hWnd, CMesh* pMeshCom, CTransform* pTransformCom, _float& fDistance)
{
	POINT	ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// 뷰포트 -> 투영
	_vector vMousePos;

	D3D11_VIEWPORT		ViewportDesc{};
	_uint				iNumViewports = { 1 };

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	vMousePos = XMVectorSet(ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f,
							ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f,
							0.f, 1.f);
	
	// 투영 -> 뷰스페이스
	_float4x4 ProjInverseMatrix;
	ProjInverseMatrix = m_pGameInstance->Get_InverseTransform(D3DTS::PROJ);
	vMousePos = XMVector3TransformCoord(vMousePos, XMLoadFloat4x4(&ProjInverseMatrix));
	
	// 뷰 스페이스 -> 월드
	_float4x4 ViewInverseMatrix;
	ViewInverseMatrix = m_pGameInstance->Get_InverseTransform(D3DTS::VIEW);

	_vector	vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_vector vRayDir = vMousePos - vRayPos;

	vRayPos = XMVector3TransformCoord(vRayPos, XMLoadFloat4x4(&ViewInverseMatrix));
	vRayDir = XMVector3TransformNormal(vRayDir, XMLoadFloat4x4(&ViewInverseMatrix));

	// 월드 -> 로컬
	_matrix WorldInverseMatrix = pTransformCom->Get_WorldMatrixInverse();

	vRayPos = XMVector3TransformCoord(vRayPos, WorldInverseMatrix);
	vRayDir = XMVector3TransformNormal(vRayDir, WorldInverseMatrix);

	_float	fDist(0.f);
	_ulong	dwVtxIdx[3] = {};

	const _float3* pMeshVtxPos = pMeshCom->m_pVertexPositions;
	const _uint* pIndices = pMeshCom->m_pIndices;
	const _uint	iNumVertices = pMeshCom->m_iNumVertices;
	const _uint	iNumIndices = pMeshCom->m_iNumIndices;

	for (_uint i = 0; i < iNumIndices / 3; i++)
	{
		_uint idx0 = pIndices[i * 3 + 0];
		_uint idx1 = pIndices[i * 3 + 1];
		_uint idx2 = pIndices[i * 3 + 2];

		if (TriangleTests::Intersects(vRayPos,
									  XMVector3Normalize(vRayDir),
									  XMLoadFloat3(&pMeshVtxPos[idx0]),
									  XMLoadFloat3(&pMeshVtxPos[idx1]),
									  XMLoadFloat3(&pMeshVtxPos[idx2]),
									  fDist))
		{
			fDistance = fDist;
			return XMVectorAdd(vRayPos, XMVectorScale(XMVector3Normalize(vRayDir), fDist));
		}
	}

	return XMVectorSet(0.f, 0.f, 0.f, 0.f);
}

CCalculator* CCalculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCalculator* pInstance = new CCalculator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCalculator::Clone(void* pArg)
{
	return nullptr;
}

void CCalculator::Free()
{
	__super::Free();

	//Safe_Release(m_pPipeLine);

}
