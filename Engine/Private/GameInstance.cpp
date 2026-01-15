#include "GameInstance.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Light_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Collision_Manager.h"
#include "Target_Manager.h"
#include "Font_Manager.h"
#include "UI_Manager.h"
#include "Sound_Manager.h"

#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
};

#pragma region ENGINE
HRESULT	CGameInstance::Initialize_Engine(EngineDesc& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext)
{
	srand(_uint(time(NULL)));

	// 그래픽 디바이스 초기화
	m_pGraphicDevice = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppDeviceContext);
	if (m_pGraphicDevice == nullptr)
		return E_FAIL;

	// 인풋 디바이스 초기화
	m_pInputDevice = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (m_pInputDevice == nullptr)
		return E_FAIL;

	// 사운드 디바이스 초기화
	
	// 폰트 매니저 초기화
	m_pFontManager = CFont_Manager::Create(*ppDevice, *ppDeviceContext);
	if (m_pFontManager == nullptr)
		return E_FAIL;

	// 타이머 매니저 초기화
	m_pTimerManager = CTimer_Manager::Create();
	if (m_pTimerManager == nullptr)
		return E_FAIL;

	// 레벨 매니저 초기화
	m_pLevelManager = CLevel_Manager::Create();
	if (m_pLevelManager == nullptr)
		return E_FAIL;

	// 프로토타입 매니저 초기화
	m_pPrototypeManager = CPrototype_Manager::Create(EngineDesc.iLevelNum);
	if (m_pPrototypeManager == nullptr)
		return E_FAIL;

	// 오브젝트 매니저 초기화
	m_pObjectManager = CObject_Manager::Create(*ppDevice, *ppDeviceContext, EngineDesc.iLevelNum);
	if (m_pObjectManager == nullptr)
		return E_FAIL;

	// 타겟 매니저 초기화
	m_pTargetManager = CTarget_Manager::Create(*ppDevice, *ppDeviceContext);
	if (m_pTargetManager == nullptr)
		return E_FAIL;
	
	// 렌더러 초기화
	m_pRenderer = CRenderer::Create(*ppDevice, *ppDeviceContext);
	if (m_pRenderer == nullptr)
		return E_FAIL;

	// UI 매니저 초기화
	m_pUIManager = CUI_Manager::Create();
	if (m_pUIManager == nullptr)
		return E_FAIL;

	// 파이프라인 초기화
	m_pPipeLine = CPipeLine::Create();
	if (m_pPipeLine == nullptr)
		return E_FAIL;

	// 빛 매니저 초기화
	m_pLightManager = CLight_Manager::Create();
	if (m_pLightManager == nullptr)
		return E_FAIL;

	// 충돌 매니저 초기화
	m_pCollisionManager = CCollision_Manager::Create();
	if (m_pCollisionManager == nullptr)
		return E_FAIL;

	// 사운드 매니저 초기화
	m_pSoundManager = CSound_Manager::Create();
	if (m_pSoundManager == nullptr)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(const _float& fTimeDelta)
{
	m_pInputDevice->Update();
	m_pObjectManager->Update_Priority(fTimeDelta);
	m_pPipeLine->Update();
	m_pObjectManager->Update(fTimeDelta);
	m_pObjectManager->Update_Late(fTimeDelta);
	m_pLevelManager->Update_Level(fTimeDelta);
	m_pSoundManager->Update();
}

HRESULT CGameInstance::Draw_Begin(const _float4* pClearColor)
{
	if (FAILED(m_pGraphicDevice->Clear_BackBuffer_View(pClearColor)))
		return E_FAIL;

	if (FAILED(m_pGraphicDevice->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	m_pRenderer->Draw();

	m_pLevelManager->Render_Level();

	return S_OK;
}

HRESULT CGameInstance::Draw_End()
{
	if (FAILED(m_pGraphicDevice->Present()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelID)
{
	// 자원을 지운다.
	m_pObjectManager->Clear(iLevelID);
	m_pPrototypeManager->Clear(iLevelID);
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	_float fRandNormal = static_cast<_float>(rand()) / RAND_MAX;

	return (fMax - fMin) * fRandNormal + fMin;
}
#pragma endregion

#pragma region INPUT_MANAGER
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInputDevice->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInputDevice->Get_DIMouseState(eMouse);
}

_bool CGameInstance::Get_MouseBtnDown(MOUSEKEYSTATE eMouse)
{
	return m_pInputDevice->Get_MouseBtnDown(eMouse);
}

_bool CGameInstance::Get_MouseBtnHold(MOUSEKEYSTATE eMouse)
{
	return m_pInputDevice->Get_MouseBtnHold(eMouse);
}

_bool CGameInstance::Get_MouseBtnUp(MOUSEKEYSTATE eMouse)
{
	return m_pInputDevice->Get_MouseBtnUp(eMouse);
}

// 현재 마우스의 특정 축 좌표를 반환
_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInputDevice->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Get_KeyDown(_ubyte dwKey)
{
	return m_pInputDevice->Get_KeyDown(dwKey);
}

_bool CGameInstance::Get_KeyUp(_ubyte dwKey)
{
	return m_pInputDevice->Get_KeyUp(dwKey);
}

_bool CGameInstance::Get_KeyHold(_ubyte dwKey)
{
	return m_pInputDevice->Get_KeyHold(dwKey);
}
#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	return m_pTimerManager->Get_TimeDelta(pTimerTag);
}

_uint CGameInstance::Get_FPS(const _tchar* pTimerTag)
{
	return m_pTimerManager->Get_FPS(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	return m_pTimerManager->Add_Timer(pTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _tchar* pTimerTag)
{
	m_pTimerManager->Compute_TimeDelta(pTimerTag);
}
#pragma endregion

#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Change_Level(_uint iCurLevelID, class CLevel* pCurLevel)
{
	m_pLevelManager->Change_Level(iCurLevelID, pCurLevel);

	return S_OK;
}
const _uint CGameInstance::Get_CurLevelID()
{
	return m_pLevelManager->Get_CurLevelID();
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelNum, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototypeManager->Add_Prototype(iLevelNum, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelNum, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototypeManager->Clone_Prototype(ePrototypeID, iLevelNum, strPrototypeTag, pArg);
}
map<const _wstring, CBase*> CGameInstance::Get_Prototypes(_uint iLevelID)
{
	return m_pPrototypeManager->Get_Prototypes(iLevelID);
}
#pragma endregion

#pragma region OBJECT_MANAGER
CGameObject* CGameInstance::Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strProtoTag,
	_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObjectManager->Add_GameObject_To_Layer(iProtoLevelIndex, strProtoTag, iLayerLevelIndex, strLayerTag, pArg);
}

void CGameInstance::Delete_GameObject_From_Layer(CGameObject* pGameObject, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	m_pObjectManager->Delete_GameObject_From_Layer(pGameObject, iLayerLevelIndex, strLayerTag);
}

map<const _wstring, class CLayer*>* CGameInstance::Get_Layers()
{
	return m_pObjectManager->Get_Layers();
}

list<class CGameObject*> CGameInstance::Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObjectManager->Get_ObjectList(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Get_Player(_uint iLevelIndex)
{
	return m_pObjectManager->Get_Player(iLevelIndex);
}
CGameObject* CGameInstance::Get_Object(_uint iLevelIndex, const _wstring& strLayerTag, const _tchar* strObjectName)
{
	return m_pObjectManager->Get_Object(iLevelIndex, strLayerTag, strObjectName);
}
#pragma endregion

#pragma region LIGHT_MANAGER
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLightManager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLightManager->Add_Light(LightDesc);
}
void CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	m_pLightManager->Render(pShader, pVIBuffer);
}
#pragma endregion

#pragma region COLLISION_MANAGER
void CGameInstance::Check_Collision(list<CGameObject*> pDstList, list<CGameObject*> pSrcList)
{
	m_pCollisionManager->Check_Collision(pDstList, pSrcList);
}
#pragma endregion

#pragma region RENDERER
HRESULT CGameInstance::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject* pObj)
{
	return m_pRenderer->Add_RenderObject(eRenderGroup, pObj);
}

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}
#endif // _DEBUG

#pragma endregion

#pragma region PIPELINE
HRESULT CGameInstance::Bind_PipeLineMatrix(CShader* pShader, const _char* pConstantName, D3DTS eTransformMatrix)
{
	return m_pPipeLine->Bind_PipeLineMatrix(pShader, pConstantName, eTransformMatrix);
}

HRESULT CGameInstance::Bind_PipeLineInverseMatrix(CShader* pShader, const _char* pConstantName, D3DTS eTransformMatrix)
{
	return m_pPipeLine->Bind_PipeLineInverseMatrix(pShader, pConstantName, eTransformMatrix);
}

HRESULT CGameInstance::Bind_CamPosition(CShader* pShader, const _char* pConstant)
{
	return m_pPipeLine->Bind_CamPosition(pShader, pConstant);
}

void CGameInstance::Set_Transform(D3DTS eTransformMatrix, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eTransformMatrix, TransformMatrix);
}
const _float4x4* CGameInstance::Get_Transform(D3DTS eTransformMatrix)
{
	return m_pPipeLine->Get_Transform(eTransformMatrix);
}

const _float4x4* CGameInstance::Get_InverseTransform(D3DTS eTransformMatrix)
{
	return m_pPipeLine->Get_InverseTransform(eTransformMatrix);
}
const _float4* CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}
#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTargetManager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTargetManager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag)
{
	return m_pTargetManager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTargetManager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTargetManager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTargetManager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Debug_RT_Render(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTargetManager->Render(strMRTTag, pShader, pVIBuffer);
}
#endif // _DEBUG
#pragma endregion

#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFontManager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
	return m_pFontManager->Draw_Text(strFontTag, pText, vPosition, vColor);
}

HRESULT CGameInstance::Draw_Text3D(const _wstring& strFontTag, const _tchar* pText, const _fvector vPosition, _fvector vColor)
{
	return m_pFontManager->Draw_Text3D(strFontTag, pText, vPosition, vColor);
}
#pragma endregion

#pragma region UI_MANAGER
HRESULT CGameInstance::Add_UI(_wstring strUITag, CUIObj* pUIObj)
{
	return m_pUIManager->Add_UI(strUITag, pUIObj);
}

CUIObj* CGameInstance::Find_UI(_wstring strUITag)
{
	return m_pUIManager->Find_UI(strUITag);
}

void CGameInstance::Show_UI(_wstring strUITag)
{
	return m_pUIManager->Show_UI(strUITag);
}

void CGameInstance::Hide_UI(_wstring strUITag)
{
	return m_pUIManager->Hide_UI(strUITag);
}
#pragma endregion

#pragma region SOUND_MANAGER
void CGameInstance::PlaySoundW(const _wstring& soundKey, CHANNELID eID, float fVolume)
{
	return m_pSoundManager->PlaySoundW(soundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(const _wstring& soundKey, float fVolume)
{
	return m_pSoundManager->PlayBGM(soundKey, fVolume);
}

void CGameInstance::StopSound(CHANNELID eID)
{
	return m_pSoundManager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	return m_pSoundManager->StopAll();
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	return m_pSoundManager->SetChannelVolume(eID, fVolume);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pSoundManager);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pFontManager);
	Safe_Release(m_pCollisionManager);
	Safe_Release(m_pLightManager);
	Safe_Release(m_pTimerManager);
	Safe_Release(m_pLevelManager);
	Safe_Release(m_pObjectManager);
	Safe_Release(m_pPrototypeManager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTargetManager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInputDevice);
	Safe_Release(m_pGraphicDevice);

	CGameInstance::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();


}