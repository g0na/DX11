#pragma once
#include "Base.h"

// 뼈 : aiBone, aiNode, aiNodeAnim

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT Initialize_Binary(ifstream& file);
	void Update_CombinedTransformMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

public:
	HRESULT Write_To_Binary(ofstream& file);
	HRESULT Read_From_Binary(ifstream& file);

public:
	_bool Compare_Name(const _char* pName)
	{
		return !strcmp(pName, m_szName);
	}

	_matrix Get_CombinedTransformationMatrix()
	{
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	_float4x4 Get_TransformationMatrix()
	{
		return m_TransformationMatrix;
	}

	_int Get_ParentBoneIndex()
	{
		return m_iParentBoneIndex;
	}

	void Set_TransformationMatrix(const _float4x4 TransformationMatrix)
	{
		m_TransformationMatrix = TransformationMatrix;
	}

private:
	_char		m_szName[MAX_PATH] = {};
	_float4x4	m_TransformationMatrix = {};						// 현재 뼈만의 상태(크 * 자 * 이), 부모로부터 얼마나 떨어져 있고, 회전한 상태인가
	_float4x4	m_CombinedTransformationMatrix = {};				// 내 TransformationMatrix * 부모`s CombinedTransformationMatrix, 월드 공간에서의 최종 위치
	_int		m_iParentBoneIndex = {};

public:
	static CBone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	static CBone* Create_Binary(ifstream& file);
	CBone* Clone();
	virtual void Free() override;
};

NS_END