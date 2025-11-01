#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

USING(Client)

Client::CMainApp::CMainApp() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

Client::CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	// EngineDesc 초기화
	EngineDesc		m_tEngineDesc {};
	m_tEngineDesc.hInstance = g_hInst;
	m_tEngineDesc.hWnd = g_hWnd;
	m_tEngineDesc.eWinMode = WINMODE::WIN;
	m_tEngineDesc.iWinSizeX = g_iWinSizeX;
	m_tEngineDesc.iWinSizeY = g_iWinSizeY;
	m_tEngineDesc.iLevelNum = ENUM_TO_UINT(LEVELID::END);
	
	if (FAILED(m_pGameInstance->Initialize_Engine(m_tEngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_For_Static()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVELID::LOGO)))
		return E_FAIL; 

	return S_OK;
}

void CMainApp::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4	vColor = { 0.f, 0.f, 255.f, 0.f };

	if (FAILED(m_pGameInstance->Draw_Begin(vColor)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_End()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	// 텍스처를 생성하고, 파일로 저장하고, 픽셀 정보를 수정
	ID3D11Texture2D* pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc{};
	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	// 생성된 텍스처의 메모리에 CPU가 직접 접근하여 데이터를 읽고 쓸 수 있도록 함
	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;

	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint* pInitialPixels = new _uint[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pInitialPixels, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	// A B G R 순서
	pInitialPixels[0] = D3DCOLOR_ARGB(255, 255, 255, 0);

	D3D11_SUBRESOURCE_DATA	InitialData{};
	InitialData.pSysMem = pInitialPixels;
	InitialData.SysMemPitch = sizeof(_uint) * TextureDesc.Width;			// SysMemPitch : 텍스쳐 가로 한줄의 바이트 크기

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	_uint* pPixels = static_cast<_uint*>(SubResource.pData);

	for (_uint i = 0; i < 256; i++)
	{
		for (_uint j = 0; j < 256; j++)
		{
			_uint	iIndex = i * 256 + j;

			if (j < 128)
				pPixels[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}
	m_pContext->Unmap(pTexture2D, 0);

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pInitialPixels);
	Safe_Release(pTexture2D);

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVELID eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(ENUM_TO_UINT(LEVELID::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Static()
{
	// Prototype_Component_VIBuffer_Rect 추가
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_Component_Shader_VtxPosTex 추가
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;
	
	if (FAILED(pMainApp->Initialize()))
	{
		MSG_BOX("Failed to create CMainApp!");
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void Client::CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	// MainApp의 멤버를 정리한다.
	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
