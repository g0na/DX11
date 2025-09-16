#include "TimerManager.h"
#include "Timer.h"

CTimerManager::CTimerManager()
{
}

_float CTimerManager::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

HRESULT CTimerManager::Add_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimer.emplace(pTimerTag, pTimer);

	return S_OK;
}

void CTimerManager::Compute_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

CTimer* CTimerManager::Find_Timer(const _wstring pTimerTag)
{
	//auto		iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));
	auto iter = m_mapTimer.find(pTimerTag);

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

CTimerManager* CTimerManager::Create()
{
	return new CTimerManager;
}

void CTimerManager::Free()
{
	for (auto& Pair : m_mapTimer)
		Safe_Release(Pair.second);

	m_mapTimer.clear();
}