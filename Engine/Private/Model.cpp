#include "Model.h"
#include "Mesh.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent { Prototype }
	, m_eModelType { Prototype.m_eModelType }
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_vecMeshes { Prototype.m_vecMeshes }
{
	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);
}

HRESULT CModel::Initialize_Prototype(MODEL eModelType, const _char* pModelFilePath)
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

	m_eModelType = eModelType;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render()
{
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		m_vecMeshes[i]->Bind_Resource();
		m_vecMeshes[i]->Render();
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i]);
		if (pMesh == nullptr)
			return E_FAIL;
		
		m_vecMeshes.push_back(pMesh);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eModelType, const char* pModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath)))
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

	m_Importer.FreeScene();
}
