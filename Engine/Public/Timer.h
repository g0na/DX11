#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	_float Get_TimeDelta() const { return m_fTimeDelta; }
	_uint	Get_FPS() const { return m_iCurrentFPS; }

public:
	HRESULT		Ready_Timer();
	void		Update_Timer();

private:
	LARGE_INTEGER		m_FrameTime = {};
	LARGE_INTEGER		m_FixTime = {};
	LARGE_INTEGER		m_LastTime = {};
	LARGE_INTEGER		m_CpuTick = {};

	_float				m_fTimeDelta = {};
	_float				m_fTimeElapsed = {};

	_uint				m_iFrameCnt = {};
	_uint				m_iCurrentFPS = {};
	

public:
	static		CTimer* Create();
	virtual void	Free();
};

NS_END