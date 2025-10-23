#include "Renderer.h"
#include "GameObject.h"
#include "Transform.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject* pObj)
{
	// 렌더 인덱스가 초과했거나 오브젝트가 널 포인터라면 추가 불가
	if (eRenderGroup >= RENDERGROUP::END ||
		pObj == nullptr)
		return E_FAIL;

	// 그게 아니라면 리스트에 삽입
	m_listRenderObj[ENUM_TO_UINT(eRenderGroup)].push_back(pObj);

	Safe_AddRef(pObj);

	return S_OK;
}

void CRenderer::Draw()
{
	Draw_Priority();
	Draw_Nonblend();
	Draw_Blend();
	Draw_UI();
}

void CRenderer::Draw_Priority()
{
	// Priority 리스트를 순회 하면서 그려준다.
	for (auto& RenderObject : m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::PRIORITY)])
	{
		if (RenderObject != nullptr)
			RenderObject->Render();

		Safe_Release(RenderObject);
	}
	m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::PRIORITY)].clear();
}

void CRenderer::Draw_Nonblend()
{
	// Nonblend 리스트를 순회 하면서 그려준다.
	for (auto& RenderObject : m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::NONBLEND)])
	{
		if (RenderObject != nullptr)
			RenderObject->Render();

		Safe_Release(RenderObject);
	}
	m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::NONBLEND)].clear();
}

void CRenderer::Draw_Blend()
{
	// Blend 리스트를 순회 하면서 그려준다.
	for (auto& RenderObject : m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::BLEND)])
	{
		if (RenderObject != nullptr)
			RenderObject->Render();

		Safe_Release(RenderObject);
	}
	m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::BLEND)].clear();
}

void CRenderer::Draw_UI()
{
	// UI 리스트를 순회 하면서 그려준다.
	for (auto& RenderObject : m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::UI)])
	{
		if (RenderObject != nullptr)
			RenderObject->Render();

		Safe_Release(RenderObject);
	}
	m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::UI)].clear();
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create Renderer");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();

	// 배열에 있는 렌더 리스트들을 비워준다.
	for (auto& RenderObjects : m_listRenderObj)
	{
		for (auto& RenderObject : RenderObjects)
			Safe_Release(RenderObject);

		RenderObjects.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}