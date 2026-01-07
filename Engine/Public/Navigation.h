#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		const _float4x4* pParentMatrix = { nullptr };
		_int iCurrentCellIndex = { -1 };
	}NAVIGATION_DESC;

	typedef struct tagNvmHeader
	{
		_uint iUnknown;
		_uint iVertexCount;
		_uint iVertexOffset;
		_uint iCellCount;
		_uint iCellOffset;
		_uint iDataEnd;
	}NVM_HEADER;


	typedef struct tagNvmCell
	{
		_int iVertexIndices[3];
		_int iNeighborIndices[3];
		_uint iFlags;
	}NVM_CELL;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	_uint	Get_CurCellIndex() { return m_iCurrentCellIndex; }

public:
	virtual HRESULT Initialize_Prototype(const vector<const _tchar*>& pNavigationData);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Load_NVM(const _tchar* pNavigationData, _uint iCellOffset);
	_vector SetOn_Navigation(_fvector vWorldPos);
	_vector Move(_fvector vCurrentPos, _fvector vTargetPos);
	_bool CanMove(_fvector vResultPos);

private:
	HRESULT SetUp_Neighbors(const _tchar* pNavigationNeighborData = nullptr);
	HRESULT SetUp_BorderNeighbors();

#ifdef _DEBUG
public:
	HRESULT Render();

	class CShader* m_pShader = { nullptr };
#endif // _DEBUG

private:
	vector<class CCell*>		m_vecCells;
	_uint						m_iNumCells = {};
	_int						m_iCurrentCellIndex = { -1 };

	static const _float4x4*		m_pParentMatrix;

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<const _tchar*>& pNavigationData);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END