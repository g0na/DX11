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
	_uint Get_NumMeshes() const	{ return m_iNumMeshes; }

	_int Get_BoneIndex(const _char* pBoneName) const;

	class CMesh* Get_Mesh(_uint iIndex) { return m_vecMeshes[iIndex]; }

	_bool is_AnimFinished() const { return m_bIsAnimFinished; }

	void Set_Animation(_uint iAnimationIndex, _bool isLoop = true);

public:
	virtual HRESULT Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex = 0);
	HRESULT Bind_Bones(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	void Play_Animation(_float fTimeDelta);

public:
	HRESULT Load_FromAssbin(const _char* pModelFilePath, MODEL eModelType, _fmatrix PreTransformMatrix);
	HRESULT Load_FromFBX(MODEL eModelType, const _char* pModelFilePath, const char* pAssbinFilePath, _fmatrix PreTransformMatrix);

private:
	Assimp::Importer				m_Importer = {};
	Assimp::Exporter				m_Exporter = {};
	const aiScene*					m_pAIScene = { nullptr };

private:
	MODEL							m_eModelType = {};
	_uint							m_iNumMeshes = {};
	vector<class CMesh*>			m_vecMeshes;
	_float4x4						m_PreTransformMatrix = {};

	_uint							m_iNumMaterials = {};
	vector<class CMaterial*>		m_vecMaterials;

	_uint							m_iNumAnimations = {};
	_uint							m_iCurrentAnimIndex = {};
	vector<class CAnimation*>		m_vecAnimations;
	_bool							m_bIsAnimLoop = { false };
	_bool							m_bIsAnimFinished = { false };

	// 현재 모델에게 영향을 주는 전체 뼈들 : 뼈들은 하나하나 독립적이지 않고, 부모 자식관계로 엮여있다.
	vector<class CBone*>			m_vecBones;

public:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END