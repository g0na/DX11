#include "GameInstance.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Light_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
};

#pragma region ENGINE
HRESULT	CGameInstance::Initialize_Engine(EngineDesc& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext)
{
	// 그래픽 디바이스 초기화
	m_pGraphicDevice = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppDeviceContext);
	if (m_pGraphicDevice == nullptr)
		return E_FAIL;

	// 인풋 디바이스 초기화
	m_pInputDevice = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (m_pInputDevice == nullptr)
		return E_FAIL;

	// 사운드 디바이스 초기화
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
	
	// 렌더러 초기화
	m_pRenderer = CRenderer::Create(*ppDevice, *ppDeviceContext);
	if (m_pRenderer == nullptr)
		return E_FAIL;

	// 파이프라인 초기화
	m_pPipeLine = CPipeLine::Create();
	if (m_pPipeLine == nullptr)
		return E_FAIL;

	// 빛 매니저 초기화
	m_pLightManager = CLight_Manager::Create();
	if (m_pLightManager == nullptr)
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
}

HRESULT CGameInstance::Draw_Begin(const _float4& vColor)
{
	if (FAILED(m_pGraphicDevice->Clear_BackBuffer_View(&vColor)))
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

CGameObject* CGameInstance::Get_Player(_uint iLevelIndex)
{
	return m_pObjectManager->Get_Player(iLevelIndex);
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
#pragma endregion


#pragma region RENDERER
HRESULT CGameInstance::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject* pObj)
{
	return m_pRenderer->Add_RenderObject(eRenderGroup, pObj);
}
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

_float4x4 CGameInstance::Get_InverseTransform(D3DTS eTransformMatrix)
{
	return m_pPipeLine->Get_InverseTransform(eTransformMatrix);
}

#pragma region


void CGameInstance::Release_Engine()
{
	Safe_Release(m_pLightManager);
	Safe_Release(m_pTimerManager);
	Safe_Release(m_pLevelManager);
	Safe_Release(m_pObjectManager);
	Safe_Release(m_pPrototypeManager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInputDevice);
	Safe_Release(m_pGraphicDevice);

	CGameInstance::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();


}