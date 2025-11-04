#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCalculator final : public CComponent
{
private:
	explicit CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCalculator() = default;

public:
	HRESULT			Initialize_Prototype() override;
	HRESULT			Initialize(void* pArg) override;

	//_float			Compute_HeightOnTerrain(const _vector* pPos,
	//										const _vector* pTerrainVtxPos,
	//										const _ulong& dwCntX,
	//										const _ulong& dwCntZ,
	//										const _ulong& dwVtxItv = VTXITV);

	_vector			Picking_OnMesh(HWND hWnd, class CMesh* pMeshCom, class CTransform* pTerrainTransformCom);

private:
	class CPipeLine*	m_pPipeLine = { nullptr };

public:
	static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END