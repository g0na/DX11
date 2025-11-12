#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"

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
	//, m_vecBones { Prototype.m_vecBones }
	//, m_vecAnimations{ Prototype.m_vecAnimations }
{
	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_vecMaterials)
		Safe_AddRef(pMaterial);

	for (auto& pPrototypeBone : Prototype.m_vecBones)
		m_vecBones.push_back(pPrototypeBone->Clone());

	for (auto* pPrototypeAnimation : Prototype.m_vecAnimations)
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

void CModel::Set_Animation(_uint iAnimationIndex, _bool isLoop)
{
	m_iCurrentAnimIndex = iAnimationIndex;
	m_bIsAnimLoop = isLoop;
}

//HRESULT CModel::Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
//{
//	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };
//
//	if (MODEL::NONANIM == eModelType)
//		iFlag |= aiProcess_PreTransformVertices;
//
//	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
//	if (m_pAIScene == nullptr)
//	{
//		MSG_BOX("Failed to Load Model");
//		return E_FAIL;
//	}
//
//	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
//
//	m_eModelType = eModelType;
//
//	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
//		return E_FAIL;
//
//	if (FAILED(Ready_Meshes()))
//		return E_FAIL;
//
//	if (FAILED(Ready_Materials(pModelFilePath)))
//		return E_FAIL;
//
//	if (FAILED(Ready_Animations()))
//		return E_FAIL;
//
//	return S_OK;
//}

HRESULT CModel::Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	return Load_FromFBX(eModelType, pModelFilePath, PreTransformMatrix);
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
	// m_iCurrentAnimIndex에 해당하는 애니메이션 중, 현재 재생 시간에 맞는 상태행렬(TransformationMatrix)을 실제 뼈에게 전달해준다.
	m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_bIsAnimLoop);

	// 위에서 갱신해준 뼈들의 TransformationMatrix를 기반으로 실제 뼈의 상태행렬(CombinedTransformationMatrix)을 만들어준다.
	for (auto& pBone : m_vecBones)
	{
		// 각 뼈의 월드 변환을 계산한다.
		pBone->Update_CombinedTransformMatrix(m_vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
}

HRESULT CModel::Load_FromFBX(MODEL eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	if (MODEL::NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (m_pAIScene == nullptr)
	{
		MSG_BOX("Failed to Load Model");
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	m_eModelType = eModelType;

	// 바이너리 파일 경로 가져오기
	_char szDir[MAX_PATH] = {};
	_char szFileName[MAX_PATH] = {};

	_splitpath_s(pModelFilePath, nullptr, NULL, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, NULL);

	strcpy_s(m_szBinFilePath, szDir);
	strcat_s(m_szBinFilePath, szFileName);
	strcat_s(m_szBinFilePath, ".bin");

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	// 바이너리 파일 관련
	fs::path p(m_szBinFilePath);

	if (fs::exists(p))			// 바이너리 파일이 존재하면 읽어오기
	{
		ifstream fileMesh(m_szBinFilePath, ios::binary);

		if (fileMesh.is_open())
			fileMesh.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));

		fileMesh.close();

		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PreTransformMatrix), m_szBinFilePath);
			if (pMesh == nullptr)
				return E_FAIL;

			m_vecMeshes.push_back(pMesh);
			pMesh->Write_To_Binary(m_szBinFilePath);
		}
	}
	else if (!fs::exists(p))	// 바이너리 파일이 없다면 assimp -> bin 쓰기
	{
		ofstream fileMesh(m_szBinFilePath, ios::binary);

		m_iNumMeshes = m_pAIScene->mNumMeshes;

		fileMesh.write(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));

		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PreTransformMatrix), m_szBinFilePath);
			if (pMesh == nullptr)
				return E_FAIL;

			m_vecMeshes.push_back(pMesh);
			pMesh->Write_To_Binary(m_szBinFilePath);
		}

		fileMesh.close();
	}

	//for (size_t i = 0; i < m_iNumMeshes; i++)
	//{
	//	CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PreTransformMatrix));
	//	if (pMesh == nullptr)
	//		return E_FAIL;
	//	
	//	m_vecMeshes.push_back(pMesh);
	//}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	m_vecMaterials.reserve(m_iNumMaterials);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], pModelFilePath, m_pAIScene);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_vecMaterials.push_back(pMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentBoneIndex);
	if (pBone == nullptr)
		return E_FAIL;

	m_vecBones.push_back(pBone);

	_int	iPIndex = (_int)m_vecBones.size() - 1;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], iPIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (pAnimation == nullptr)
			return E_FAIL;

		m_vecAnimations.push_back(pAnimation);
	}

	return S_OK;
}

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
