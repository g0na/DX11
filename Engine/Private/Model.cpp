#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent { Prototype }
	, m_eModelType { Prototype.m_eModelType }
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_vecMeshes { Prototype.m_vecMeshes }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_vecMaterials { Prototype.m_vecMaterials }
{
	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_vecMaterials)
		Safe_AddRef(pMaterial);
}

HRESULT CModel::Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
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

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	return S_OK;
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

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (pMesh == nullptr)
			return E_FAIL;
		
		m_vecMeshes.push_back(pMesh);
	}

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

	m_Importer.FreeScene();
}
