#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	_uint iNumViewports = { 1 };
	D3D11_VIEWPORT ViewPortDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
	
	// 후처리 쉐이더를 위한 렌더 타겟들을 생성한다.
	// Target_Diffuse
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Normal
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Depth
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Shade
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Specular
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For MRT_GameObject
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Depth"))))
		return E_FAIL;

	// For MRT_LightAcc
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPortDesc.Width, ViewPortDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (m_pVIBuffer == nullptr)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (m_pShader == nullptr)
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 450.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
#endif // _DEBUG

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
	Draw_Lights();
	Draw_Combined();
	Draw_NonLight();
	Draw_Blend();
	Draw_UI();

#ifdef _DEBUG
	Render_Debug();
#endif // _DEBUG
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
	// Diffuse + Normal
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObject"))))
		return;
	
	// Nonblend 리스트를 순회 하면서 그려준다.
	for (auto& RenderObject : m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::NONBLEND)])
	{
		if (RenderObject != nullptr)
			RenderObject->Render();

		Safe_Release(RenderObject);
	}
	m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::NONBLEND)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Draw_Lights()
{
	// Shade + Specular
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_InverseTransform(D3DTS::VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_InverseTransform(D3DTS::PROJ))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return;
	//if (FAILED(m_pGameInstance->Bind_PipeLineInverseMatrix(m_pShader, "g_ViewMatrixInv", D3DTS::VIEW)))
	//	return;
	//if (FAILED(m_pGameInstance->Bind_PipeLineInverseMatrix(m_pShader, "g_ProjMatrixInv", D3DTS::PROJ)))
	//	return;
	//if (FAILED(m_pGameInstance->Bind_CamPosition(m_pShader, "g_vCamPosition")))
	//	return;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Draw_Combined()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;

	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::COMBINED))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
}

void CRenderer::Draw_NonLight()
{
	for (auto& RenderObject : m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::NONLIGHT)])
	{
		if (RenderObject != nullptr)
			RenderObject->Render();

		Safe_Release(RenderObject);
	}
	m_listRenderObj[ENUM_TO_UINT(RENDERGROUP::NONLIGHT)].clear();
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

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_listDebugComponents.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}

void CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_listDebugComponents)
	{
		if (pDebugCom != nullptr)
		{
			pDebugCom->Render();
			Safe_Release(pDebugCom);
		}
	}
	m_listDebugComponents.clear();

	if (FAILED(m_pVIBuffer->Bind_Resource()))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Debug_RT_Render(TEXT("MRT_GameObject"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Debug_RT_Render(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return;
}
#endif // _DEBUG

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

#ifdef _DEBUG
	for (auto& pDebugCom : m_listDebugComponents)
		Safe_Release(pDebugCom);
	m_listDebugComponents.clear();
#endif // _DEBUG

	// 배열에 있는 렌더 리스트들을 비워준다.
	for (auto& RenderObjects : m_listRenderObj)
	{
		for (auto& RenderObject : RenderObjects)
			Safe_Release(RenderObject);

		RenderObjects.clear();
	}

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}