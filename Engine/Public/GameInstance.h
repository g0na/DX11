#pragma once
#include "Prototype_Manager.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final :  public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region ENGINE
public:
	HRESULT					Initialize_Engine(EngineDesc& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext);
	void					Update_Engine(const _float& fTimeDelta);
	HRESULT					Draw_Begin(const _float4* pClearColor);
	HRESULT					Draw();
	HRESULT					Draw_End();
	void					Clear(_uint iLevelID);		// 정해진 레벨의 자원을 정리한다.
	_float					Random(_float fMin, _float fMax);		// 최대 최소를 정해서 무작위 실수를 반환하는 함수
#pragma endregion

#pragma region INPUT_MANAGER
public:
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_bool	Get_MouseBtnDown(MOUSEKEYSTATE eMouse);
	_bool	Get_MouseBtnHold(MOUSEKEYSTATE eMouse);
	_bool	Get_MouseBtnUp(MOUSEKEYSTATE eMouse);

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	_bool	Get_KeyDown(_ubyte dwKey);
	_bool	Get_KeyUp(_ubyte dwKey);
	_bool	Get_KeyHold(_ubyte dwKey);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	_uint		Get_FPS(const _tchar* pTimerTag);
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	void		Compute_TimeDelta(const _tchar* pTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT	Change_Level(_uint iCurLevelID, class CLevel* pCurLevel);
	const _uint Get_CurLevelID();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelNum, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelNum, const _wstring& strPrototypeTag, void* pArg = nullptr);
	map<const _wstring, CBase*> Get_Prototypes(_uint iLevelID);
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	class CGameObject* Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strProtoTag,
		_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);

	void Delete_GameObject_From_Layer(class CGameObject* pGameObject, _uint iLayerLevelIndex, const _wstring& strLayerTag);

	map<const _wstring, class CLayer*>* Get_Layers();

	list<class CGameObject*> Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag);

	class CGameObject* Get_Player(_uint iLevelIndex);

	class CGameObject* Get_Object(_uint iLevelIndex, const _wstring& strLayerTag, const _tchar* strObjectName);
#pragma endregion

#pragma region LIGHT_MANAGER
public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	void Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#pragma endregion

#pragma region COLLISION_MANAGER
public:
	void Check_Collision(list<CGameObject*> pDstList, list<CGameObject*> pSrcList);
#pragma endregion

#pragma region RENDERER
public:
	HRESULT	Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pObj);

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif // _DEBUG
#pragma endregion

#pragma region PIPELINE
	HRESULT Bind_PipeLineMatrix(class CShader* pShader, const _char* pConstantName, D3DTS eTransformMatrix);
	HRESULT Bind_PipeLineInverseMatrix(class CShader* pShader, const _char* pConstantName, D3DTS eTransformMatrix);
	HRESULT Bind_CamPosition(class CShader* pShader, const _char* pConstant);
	void Set_Transform(D3DTS eTransformMatrix, _fmatrix TransformMatrix);
	const _float4x4* Get_Transform(D3DTS eTransformMatrix);
	const _float4x4* Get_InverseTransform(D3DTS eTransformMatrix);
	const _float4* Get_CamPosition();
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_RT_Render(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor);
	HRESULT Draw_Text3D(const _wstring& strFontTag, const _tchar* pText, const _fvector vPosition, _fvector vColor);
#pragma endregion

#pragma region UI_MANAGER
	HRESULT		Add_UI(_wstring strUITag, class CUIObj* pUIObj);
	CUIObj*		Find_UI(_wstring strUITag);
	void		Show_UI(_wstring strUITag);
	void		Hide_UI(_wstring strUITag);
#pragma endregion

#pragma region SOUND_MANAGER
	void PlaySoundW(const _wstring& soundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const _wstring& soundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);
#pragma endregion


private:
	class CGraphic_Device*		m_pGraphicDevice = { nullptr };
	class CInput_Device*		m_pInputDevice = { nullptr };
	class CTimer_Manager*		m_pTimerManager = { nullptr };
	class CLevel_Manager*		m_pLevelManager = { nullptr };
	class CObject_Manager*		m_pObjectManager = { nullptr };
	class CPrototype_Manager*	m_pPrototypeManager = { nullptr };
	class CLight_Manager*		m_pLightManager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CCollision_Manager*	m_pCollisionManager = { nullptr };
	class CTarget_Manager*		m_pTargetManager = { nullptr };
	class CFont_Manager*		m_pFontManager = { nullptr };
	class CUI_Manager*			m_pUIManager = { nullptr };
	class CSound_Manager*		m_pSoundManager = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;

}; 

NS_END