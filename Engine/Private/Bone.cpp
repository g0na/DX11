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

	// Assimp는 row-major를 사용하지만 DirectX는 column-major를 사용해서 전치해줘야함.
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

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

void CBone::Free()
{
	__super::Free();
}
