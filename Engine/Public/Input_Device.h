#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[ENUM_TO_UINT(eMouse)];
	}

	_bool	Get_MouseBtnDown(MOUSEKEYSTATE eMouse)
	{
		// 이전 프레임에 안 눌리고, 현재 프레임에 눌렸다면
		if (!(m_tOldMouseState.rgbButtons[ENUM_TO_UINT(eMouse)] & 0x80) &&
			(m_tMouseState.rgbButtons[ENUM_TO_UINT(eMouse)] & 0x80))
			return true;
		else
			return false;
	}

	_bool	Get_MouseBtnHold(MOUSEKEYSTATE eMouse)
	{
		// 이전 프레임에도 눌리고, 현재 프레임에도 눌렸다면
		if ((m_tOldMouseState.rgbButtons[ENUM_TO_UINT(eMouse)] & 0x80) &&
			(m_tMouseState.rgbButtons[ENUM_TO_UINT(eMouse)] & 0x80))
			return true;
		else
			return false;
	}

	_bool Get_MouseBtnUp(MOUSEKEYSTATE eMouse)
	{
		// 이전 프레임에 눌렸고, 현재 프레임에는 안 눌렸다면
		if ((m_tOldMouseState.rgbButtons[ENUM_TO_UINT(eMouse)] & 0x80) &&
			!(m_tMouseState.rgbButtons[ENUM_TO_UINT(eMouse)] & 0x80))
			return true;
		else
			return false;
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + ENUM_TO_UINT(eMouseState));
	}

	_bool	Get_KeyDown(_ubyte dwKey)
	{
		return (m_byKeyState[dwKey]) && !(m_byOldKeyState[dwKey]);
	}

	_bool	Get_KeyUp(_ubyte dwKey)
	{
		return (!m_byKeyState[dwKey]) && (m_byOldKeyState[dwKey]);
	}

	_bool	Get_KeyHold(_ubyte dwKey)
	{
		return (m_byKeyState[dwKey]);
	}


public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byOldKeyState[256];
	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			m_tOldMouseState;

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void	Free(void);

};
NS_END
#endif // InputDev_h__


