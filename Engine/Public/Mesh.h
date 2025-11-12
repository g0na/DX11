#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
	friend class CCalculator;

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;
	
public:
	_uint Get_MaterialIndex() const
	{
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(MODEL eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix, const _char* pBinFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Bones(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);
	void Write_To_Binary(const _char* pBinFilePath);
	void Read_From_Binary(const _char* pBinFilePath);

private:
	_char		m_szName[MAX_PATH] = {};
	_uint		m_iMaterialIndex = {};
	_uint		m_iNumBones = {};		// 현재 메시를 구성하는 정점들에게 영향을 주는 뼈의 개수

	// 현재 메시에 영향을 주는 뼈들의 인덱스들을 모아놓는다.
	// 뼈들의 인덱스 : 모델 객체가 들고있는 전체 뼈 컨테이너의 특정 인덱스
	vector<_uint>	m_vecBoneIndices;
	_float4x4		m_BoneMatrices[g_iMaxNumBones] = {};		// 셰이더에 전달할 최종 행렬들, 정점을 본의 로컬 공간으로 변환, 모델 로드 시 한번만 계산되어 저장된다.

	// 특정 뼈를 정점에게 적용할 때, 추가 보정을 거쳐서 정점에게 상태를 전달할 수 있도록 한다.
	vector<_float4x4>	m_OffsetMatrices;

	// 버텍스 복사용 변수
	MODEL			m_eModelType = {};
	VTXMESH*		m_pNonAnimVertices = { nullptr };
	VTXANIMMESH*	m_pAnimVertices = { nullptr };

private:
	HRESULT Ready_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_For_AnimMesh(const aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix, const _char* pBinFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END