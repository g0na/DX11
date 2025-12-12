#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare(_fvector vSrcPoint, _fvector vDstPoint);
	_bool isIn(_fvector vResultPos, _int* pNeightborIndex);
	_float Compute_Height(_vector vCellPos);

public:
	_vector Get_Point(CELLPOINT ePoint) const { return XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ePoint)]); }

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) 
	{
		m_iNeighbors[ENUM_TO_UINT(eLine)] = pNeighbor->m_iIndex;
	}

	void Set_Neighbors(_int* pNeighbors)
	{
		memcpy(m_iNeighbors, pNeighbors, sizeof(_int) * 3);
	}

#ifdef _DEBUG
public:
	HRESULT Render();

private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	_float3			m_vPoints[ENUM_TO_UINT(CELLPOINT::END)] = {};
	_float3			m_vNormals[ENUM_TO_UINT(LINE::END)] = {};
	_int			m_iNeighbors[ENUM_TO_UINT(LINE::END)] = { -1, -1, -1 };
	_uint			m_iIndex = {};

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

NS_END