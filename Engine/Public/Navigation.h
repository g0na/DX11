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

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationData, const _tchar* pNavigationNeighborData);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_vector SetOn_Navigation(_fvector vWorldPos);
	_bool CanMove(_fvector vResultPos);

private:
	HRESULT SetUp_Neighbors(const _tchar* pNavigationNeighborData = nullptr);

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
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationData, const _tchar* pNavigationNeighborData = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END