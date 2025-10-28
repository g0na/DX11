#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Render();

private:
	Assimp::Importer		m_Importer = {};
	const aiScene*			m_pAIScene = { nullptr };

private:
	MODEL					m_eModelType = {};
	_uint					m_iNumMeshes = {};
	vector<class CMesh*>	m_vecMeshes;

public:
	HRESULT Ready_Meshes();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END