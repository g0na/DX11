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
	_uint Get_NumMeshes() const
	{
		return m_iNumMeshes;
	}

	class CMesh* Get_Mesh(_uint iIndex) { return m_vecMeshes[iIndex]; }

public:
	virtual HRESULT Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex = 0);

private:
	Assimp::Importer		m_Importer = {};
	const aiScene*			m_pAIScene = { nullptr };

private:
	MODEL					m_eModelType = {};
	_uint					m_iNumMeshes = {};
	vector<class CMesh*>	m_vecMeshes;
	_float4x4				m_PreTransformMatrix = {};

	_uint							m_iNumMaterials = {};
	vector<class CMaterial*>		m_vecMaterials;

public:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END