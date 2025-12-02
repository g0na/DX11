#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
//#include "iostream"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent { Prototype }
	, m_eModelType { Prototype.m_eModelType }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_vecMeshes { Prototype.m_vecMeshes }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_vecMaterials { Prototype.m_vecMaterials }
	, m_iNumAnimations { Prototype.m_iNumAnimations }
	, m_iRootBoneIndex { Prototype.m_iRootBoneIndex }
	, m_vAccumulatedMotionDelta { Prototype.m_vAccumulatedMotionDelta }
	, m_fPrevTrackPosition { Prototype.m_fPrevTrackPosition }
	, m_vPrevRootPosition { Prototype.m_vPrevRootPosition }
{
	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_vecMaterials)
		Safe_AddRef(pMaterial);

	for (auto& pPrototypeBone : Prototype.m_vecBones)
		m_vecBones.push_back(pPrototypeBone->Clone());

	for (auto& pPrototypeAnimation : Prototype.m_vecAnimations)
		m_vecAnimations.push_back(pPrototypeAnimation->Clone());
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_int iIndex = { 0 };

	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* pBone)->_bool
		{
			if (pBone->Compare_Name(pBoneName))
				return true;

			++iIndex;

			return false;
		});

	if (iter == m_vecBones.end())
		return -1;

	return iIndex;
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* pBone)->_bool
		{
			if (pBone->Compare_Name(pBoneName))
				return true;

			return false;
		});

	if (iter == m_vecBones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrixPtr();
}

void CModel::Set_Animation(_uint iAnimationIndex, _bool isLoop)
{	
	if (iAnimationIndex == m_iCurrentAnimIndex && isLoop)
		return;

	m_iPrevAnimIndex = m_iCurrentAnimIndex;
	m_iCurrentAnimIndex = iAnimationIndex;
	m_bIsAnimLoop = isLoop;
	m_bIsAnimBlend = true;
	m_fBlendTime = 0.f;
	m_fBlendDuration = 0.25f;
	m_fPrevTrackPosition = 0.f;

	m_vecPrevBoneTransforms.clear();
	m_vecPrevBoneTransforms.reserve(m_vecBones.size());

	// 이전 애니메이션 상태가 적용된 뼈의 행렬을 저장
	for (_uint i = 0; i < m_vecBones.size(); i++)
	{
		m_vecPrevBoneTransforms.push_back(m_vecBones[i]->Get_TransformationMatrix());

		_vector vScale{}, vRotation{};

		if (m_iRootBoneIndex == i)
			XMMatrixDecompose(&vScale, &vRotation, &m_vPrevRootPosition, XMLoadFloat4x4(&m_vecPrevBoneTransforms[i]));
	}

	// 현재 트랙 위치 초기화
	m_vecAnimations[m_iCurrentAnimIndex]->Reset_TrackPosition();

	// 누적 이동량 초기화 안하면 애니메이션 상태 바뀔 때 마다 이전 애니메이션에 저장된 누적 이동량이 적용되서 텔포함.
	m_vAccumulatedMotionDelta = XMVectorZero();
}

HRESULT CModel::Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	m_eModelType = eModelType;

	// 바이너리 파일 경로 가져오기
	_char szDir[MAX_PATH] = {};
	_char szFileName[MAX_PATH] = {};

	_splitpath_s(pModelFilePath, nullptr, NULL, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, NULL);

	strcpy_s(m_szBinFilePath, szDir);
	strcat_s(m_szBinFilePath, szFileName);
	strcat_s(m_szBinFilePath, ".bin");

	fs::path fileModel(m_szBinFilePath);

	if (fs::exists(fileModel))
		return Load_FromBin(PreTransformMatrix);
	else
		return Load_FromFBX(pModelFilePath, PreTransformMatrix);
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_vecMeshes[iMeshIndex]->Bind_Resource();
	m_vecMeshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint	iMaterialIndex = m_vecMeshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_vecMaterials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eMaterialType, iTextureIndex);
}

HRESULT CModel::Bind_Bones(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_vecMeshes[iMeshIndex]->Bind_Bones(pShader, pConstantName, m_vecBones);
}

void CModel::Play_Animation(_float fTimeDelta)
{
	// 애니메이션 보간 중이라면
	if (m_bIsAnimBlend)
	{
		m_fBlendTime += fTimeDelta;
		_float fBlendRatio = min(1.f, m_fBlendTime / m_fBlendDuration);

		m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_bIsAnimLoop);
		
		_float4x4	TransformationMatrix{};

		_vector		vScale{}, vRotation{}, vTranslation{};
		_vector     vLeftScale{}, vRightScale{};
		_vector     vLeftRotation{}, vRightRotation{};
		_vector     vLeftTranslation{}, vRightTranslation{};

		for (_uint i = 0; i < m_vecBones.size(); i++)
		{
			_float4x4 CurrentTransformationMatrix = m_vecBones[i]->Get_TransformationMatrix();
			XMMatrixDecompose(&vLeftScale, &vLeftRotation, &vLeftTranslation, XMLoadFloat4x4(&m_vecPrevBoneTransforms[i]));
			XMMatrixDecompose(&vRightScale, &vRightRotation, &vRightTranslation, XMLoadFloat4x4(&CurrentTransformationMatrix));

			vScale = XMVectorLerp(vLeftScale, vRightScale, fBlendRatio);
			vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fBlendRatio);
			vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fBlendRatio);

			XMStoreFloat4x4(&TransformationMatrix,
				XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));

			m_vecBones[i]->Set_TransformationMatrix(TransformationMatrix);
		}

		if (fBlendRatio >= 1.f)
			m_bIsAnimBlend = false;
	}
	else    // 보간 중이지 않다면 원래 하던데로 애니메이션 재생
	{
		// m_iCurrentAnimIndex에 해당하는 애니메이션 중, 현재 재생 시간에 맞는 상태행렬(TransformationMatrix)을 실제 뼈에게 전달해준다.
		m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_bIsAnimLoop);
	}

	// RootNode는 움직임이 없다
	// 실제로는 c2390 Armature <Darkwraith>라는 인덱스 3짜리가 루트 본이었음.
	if (m_iRootBoneIndex != -1)
	{
		_vector vCurRootPosition{}, vScale{}, vRotation{};
		_float4x4 TransformationMatrix = m_vecBones[m_iRootBoneIndex]->Get_TransformationMatrix();

		XMMatrixDecompose(&vScale, &vRotation, &vCurRootPosition, XMLoadFloat4x4(&TransformationMatrix));

		_float fCurrentTrackPosition = m_vecAnimations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition();
		// 현재 애니메이션의 재생 위치가 0으로 바뀌면 한 사이클이 끝났다는 의미
		if (fCurrentTrackPosition < m_fPrevTrackPosition)
		{
			// 이전 프레임의 로컬 위치를 누적 위치에 저장
			// ex) 루트 본이 로컬에서 0 ~ 10까지 간다 했을 때 10을 저장
			m_vAccumulatedMotionDelta = m_vPrevRootPosition;
			m_vAccumulatedMotionDelta = XMVectorSetW(m_vAccumulatedMotionDelta, 0.f);
		}
		// 한 사이클이 끝났으니 애니메이션의 이전 재생 위치를 갱신해준다.
		m_fPrevTrackPosition = fCurrentTrackPosition;

		// 루트 본의 월드 위치 계산 = 루트 본의 로컬 위치 + 루트 본의 누적 이동량
		_vector vWorldTranslation = XMVectorSetW(vCurRootPosition + m_vAccumulatedMotionDelta, 1.f);

		// Delta = 현재 실제 위치 - 이전 실제 위치
		// GameObject에 던져줘서 더할 것 이라서 w = 0으로 설정
		m_vRootMotionDelta = vWorldTranslation - m_vPrevRootPosition;
		m_vRootMotionDelta = XMVectorSetW(m_vRootMotionDelta, 0.f);

		// 루프 애니메이션 아니면 델타 없애기
		if (!m_bIsAnimLoop && m_bIsAnimFinished)
			m_vRootMotionDelta = XMVectorZero();

		// 이전 위치 업데이트
		m_vPrevRootPosition = vWorldTranslation;

		// 루트 본의 Translation을 제거한다.
		XMStoreFloat4x4(&TransformationMatrix,
			XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorZero()));
		m_vecBones[m_iRootBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
	}

	// 위에서 갱신해준 뼈들의 TransformationMatrix를 기반으로 실제 뼈의 상태행렬(CombinedTransformationMatrix)을 만들어준다.
	for (auto& pBone : m_vecBones)
	{
		// 각 뼈의 월드 변환을 계산한다.
		pBone->Update_CombinedTransformMatrix(m_vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
}

HRESULT CModel::Load_FromFBX(const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	if (MODEL::NONANIM == m_eModelType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (m_pAIScene == nullptr)
	{
		MSG_BOX("Failed to Load Model");
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	ofstream fileBin(m_szBinFilePath, ios::binary);

	if (FAILED(Ready_Bones(fileBin, m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	// 뼈 정보 바이너리 파일로 저장
	_uint iNumBones = (_uint)m_vecBones.size();
	fileBin.write(CHARCAST(&iNumBones), sizeof(_uint));

	for (auto& pBone : m_vecBones)
		pBone->Write_To_Binary(fileBin);

	if (FAILED(Ready_Meshes(fileBin)))
		return E_FAIL;

	if (FAILED(Ready_Materials(fileBin, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations(fileBin)))
		return E_FAIL;

	fileBin.close();

	return S_OK;
}

HRESULT CModel::Load_FromBin(_fmatrix PreTransformMatrix)
{
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	ifstream fileBin(m_szBinFilePath, ios::binary);

	if (FAILED(Ready_Bones(fileBin)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(fileBin)))
		return E_FAIL;

	if (FAILED(Ready_Materials(fileBin)))
		return E_FAIL;

	if (FAILED(Ready_Animations(fileBin)))
		return E_FAIL;

	fileBin.close();

	return S_OK;
}

#pragma region ASSIMP LOAD
HRESULT CModel::Ready_Meshes(ofstream& fileBin)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;
	fileBin.write(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (pMesh == nullptr)
			return E_FAIL;

		m_vecMeshes.push_back(pMesh);
	}

	// 생성된 메쉬 파일 정보를 바이너리 파일에 작성

	for (auto& pMesh : m_vecMeshes)
		pMesh->Write_To_Binary(m_eModelType, fileBin);

	return S_OK;
}

HRESULT CModel::Ready_Materials(ofstream& fileBin, const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;
	fileBin.write(CHARCAST(&m_iNumMaterials), sizeof(_uint));
	m_vecMaterials.reserve(m_iNumMaterials);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], pModelFilePath, m_pAIScene);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_vecMaterials.push_back(pMaterial);
	}

	// 생성된 메쉬 파일 정보를 바이너리 파일에 작성
	for (auto& pMaterial : m_vecMaterials)
		pMaterial->Write_To_Binary(fileBin);

	return S_OK;
}

HRESULT CModel::Ready_Bones(ofstream& fileBin, const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentBoneIndex);
	if (pBone == nullptr)
		return E_FAIL;

	m_vecBones.push_back(pBone);

	if (strstr(pBone->Get_BoneName(), "BD_A_9500 Armature"))
		m_iRootBoneIndex = 3;
	else if (m_iRootBoneIndex == -1)
		m_iRootBoneIndex = 2;

	_int	iPIndex = (_int)m_vecBones.size() - 1;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(fileBin, pAINode->mChildren[i], iPIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(ofstream& fileBin)
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;
	fileBin.write(CHARCAST(&m_iNumAnimations), sizeof(_uint));

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(fileBin, m_pAIScene->mAnimations[i], this);
		if (pAnimation == nullptr)
			return E_FAIL;

		m_vecAnimations.push_back(pAnimation);
	}

	return S_OK;
}
#pragma endregion

#pragma region BINARY LOAD
HRESULT CModel::Ready_Meshes(ifstream& fileBin)
{
	if (fileBin.is_open())
		fileBin.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create_Binary(m_pDevice, m_pContext, m_eModelType, this, fileBin);
		if (pMesh == nullptr)
			return E_FAIL;

		m_vecMeshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(ifstream& fileBin)
{
	if (fileBin.is_open())
		fileBin.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof(_uint));

	m_vecMaterials.reserve(m_iNumMaterials);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create_Binary(m_pDevice, m_pContext, fileBin);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_vecMaterials.push_back(pMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(ifstream& fileBin)
{
	_uint iNumBones = {};
	fileBin.read(CHARCAST(&iNumBones), sizeof(_uint));

	m_vecBones.reserve(iNumBones);

	for (_uint i = 0; i < iNumBones; i++)
	{
		CBone* pBone = CBone::Create_Binary(fileBin);
		if (pBone == nullptr)
			return E_FAIL;

		m_vecBones.push_back(pBone);

		if (strstr(pBone->Get_BoneName(), "BD_A_9500 Armature"))
			m_iRootBoneIndex = i;
	}

	if (m_iRootBoneIndex == -1)
		m_iRootBoneIndex = 2;

	return S_OK;
}

HRESULT CModel::Ready_Animations(ifstream& fileBin)
{
	fileBin.read(CHARCAST(&m_iNumAnimations), sizeof(_uint));

	m_vecAnimations.reserve(m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create_Binary(fileBin);
		if (pAnimation == nullptr)
			return E_FAIL;

		m_vecAnimations.push_back(pAnimation);
	}

	return S_OK;
}
#pragma endregion

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	} 

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMesh : m_vecMeshes)
		Safe_Release(pMesh);
	m_vecMeshes.clear();

	for (auto& pMaterial : m_vecMaterials)
		Safe_Release(pMaterial);
	m_vecMaterials.clear();

	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);
	m_vecBones.clear();

	for (auto& pAnimation : m_vecAnimations)
		Safe_Release(pAnimation);
	m_vecAnimations.clear();

	m_Importer.FreeScene();
}
