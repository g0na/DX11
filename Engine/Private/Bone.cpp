#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
	// Assimp 노드에서 뼈 이름 복사
	strcpy_s(m_szName, pAINode->mName.data);

	// 부모 인덱스 저장
	m_iParentBoneIndex = iParentBoneIndex;

	// Assimp는 row-major를 사용하지만 DirectX는 column-major를 사용하므로 Assimp에서 받아온 행렬을 전치해줘야함.
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CBone::Initialize_Binary(ifstream& file)
{
	if (FAILED(Read_From_Binary(file)))
		return E_FAIL;

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (m_iParentBoneIndex == -1)		// 최상위 본 일경우
	{
		// 본의 로컬 변환 * 모델의 사전 변환 행렬을 계산한다.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
	}
	else	// 자식 본 일경우
	{
		// 본의 로컬 변환 * 부모의 CombinedMatrix를 계산한다.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
	}
}

HRESULT CBone::Write_To_Binary(ofstream& file)
{
	_uint iNameLength = (_uint)strlen(m_szName);
	file.write(CHARCAST(&iNameLength), sizeof(_uint));
	file.write(m_szName, iNameLength);
	file.write(CHARCAST(&m_iParentBoneIndex), sizeof(_uint));
	file.write(reinterpret_cast<_char*>(&m_TransformationMatrix), sizeof(_float4x4));

	return S_OK;
}

HRESULT CBone::Read_From_Binary(ifstream& file)
{
	_uint iNameLength;
	file.read(CHARCAST(&iNameLength), sizeof(_uint));
	file.read(m_szName, iNameLength);
	file.read(CHARCAST(&m_iParentBoneIndex), sizeof(_uint));
	file.read(reinterpret_cast<_char*>(&m_TransformationMatrix), sizeof(_float4x4));
	
	return S_OK;
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Create_Binary(ifstream& file)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize_Binary(file)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
