#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CTimerManager :  public CBase
{
private:
	explicit CTimerManager();
	~CTimerManager() = default;

public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);

public:
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	void		Compute_TimeDelta(const _tchar* pTimerTag);

private:
	map<const _wstring, class CTimer*>		m_mapTimer;

private:
	class CTimer* Find_Timer(const _wstring pTimerTag);
	
public:
	static CTimerManager* Create();
	virtual void Free() override;
};

NS_END