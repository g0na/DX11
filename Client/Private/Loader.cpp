#include "Loader.h"	
#include "Background.h"
#include "GameInstance.h"

USING(Client)

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
	, m_pGameInstance {CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

// 보조 스레드의 작업 시작점
//_uint APIENTRY ThreadMain(void* pArg)
//{
//	// 전달받은 this를 받는다
//	CLoader* pLoader = static_cast<CLoader*>(pArg);
//
//	// 전달받은 매개 변수에 따라서 로딩을 시작한다.
//	if (FAILED(pLoader->Loading()))
//		return 1;
//
//	return 0;
//}

HRESULT CLoader::Initialize(LEVELID eLoadingLevelID)
{
	m_eLoadingLevelID = eLoadingLevelID;

	m_Thread = thread(&CLoader::Loading, this);

	// 작업 공간(임계 영역)을 준비
	//InitializeCriticalSection(&m_CriticalSection);

	// 보조 스레드를 생성하는 함수
	//m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	//if (m_hThread == 0)
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{ 
	// 보조 스레드가 작업 공간을 사용한다고 선언 (잠금)
	//EnterCriticalSection(&m_CriticalSection);
	
	// COM 라이브러리 초기화
	CoInitializeEx(nullptr, 0);

	HRESULT hr = {};

	switch (m_eLoadingLevelID)
	{
	case LEVELID::LOGO:
		hr = Loading_Logo();

		break;

	case LEVELID::GAMEPLAY:
		hr = Loading_GamePlay();

		break;
	}

	// 작업 공간 사용이 끝났음을 선언 (잠금 해제)
	//LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Logo()
{
	lock_guard<mutex> lock(m_mutex);

	lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
	// For Prototype_Component_Texture_Background
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::LOGO), TEXT("Prototype_Component_Texture_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default0.jpg"), 1))))
		return E_FAIL;
	
	lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));

	lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));

	lstrcpy(m_szFPS, TEXT("객체원형을(를) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVELID::LOGO), TEXT("Prototype_GameObject_Background"),
		CBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	lstrcpy(m_szFPS, TEXT("로딩이 완료되었슴니다."));

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_GamePlay()
{
	lock_guard<mutex> lock(m_mutex);

	lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));

	lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));

	lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));

	lstrcpy(m_szFPS, TEXT("객체원형을(를) 로딩 중 입니다."));

	lstrcpy(m_szFPS, TEXT("로딩이 완료되었슴니다."));

	m_bIsFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(LEVELID eNextLevelID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Create CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	// 보조 스레드의 작업이 완전히 마칠 때까지 기다린다.
	//WaitForSingleObject(m_hThread, INFINITE);

	// 사용했던 작업 공간 (임계 영역)을 정리한다.
	//DeleteCriticalSection(&m_CriticalSection);

	// 보조 스레드의 핸들을 정리한다.
	//CloseHandle(m_hThread);

	if (m_Thread.joinable())
		m_Thread.join();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}