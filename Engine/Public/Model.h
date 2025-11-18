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

	_vector Get_RootMotionDelta() { return m_vRootMotionDelta; }

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
	HRESULT Load_FromFBX(const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	HRESULT Load_FromBin(_fmatrix PreTransformMatrix);

private:
	Assimp::Importer				m_Importer = {};
	Assimp::Exporter				m_Exporter = {};
	const aiScene*					m_pAIScene = { nullptr };

private:
	_char							m_szBinFilePath[MAX_PATH] = {};

	MODEL							m_eModelType = {};
	_uint							m_iNumMeshes = {};
	vector<class CMesh*>			m_vecMeshes;
	_float4x4						m_PreTransformMatrix = {};

	_uint							m_iNumMaterials = {};
	vector<class CMaterial*>		m_vecMaterials;

	_uint							m_iNumAnimations = {};
	_uint							m_iCurrentAnimIndex = {};		// 현재 애니메이션
	_uint							m_iPrevAnimIndex = {};			// 이전 애니메이션
	vector<class CAnimation*>		m_vecAnimations;
	_bool							m_bIsAnimLoop = { false };
	_bool							m_bIsAnimFinished = { false };
	_bool							m_bIsAnimBlend = { false };

	// 현재 모델에게 영향을 주는 전체 뼈들 : 뼈들은 하나하나 독립적이지 않고, 부모 자식관계로 엮여있다.
	vector<class CBone*>			m_vecBones;
	vector<_float4x4>				m_vecPrevBoneTransforms;
	_float							m_fBlendTime = {};
	_float							m_fBlendDuration = {};
	_vector							m_vPrevRootPosition = {};
	_vector							m_vRootMotionDelta = {};

public:
	HRESULT Ready_Meshes(ofstream& fileBin);
	HRESULT Ready_Materials(ofstream& fileBin, const _char* pModelFilePath);
	HRESULT Ready_Bones(ofstream& fileBin, const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT Ready_Animations(ofstream& fileBin);

	HRESULT Ready_Meshes(ifstream& fileBin);
	HRESULT Ready_Materials(ifstream& fileBin);
	HRESULT Ready_Bones(ifstream& fileBin);
	HRESULT Ready_Animations(ifstream& fileBin);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END