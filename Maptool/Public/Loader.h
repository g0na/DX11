#pragma once

#include "Maptool_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Maptool)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVELID eLoadingLevelID);
	HRESULT Loading();

	_bool	isFinished() const { return m_bIsFinished; }
	
	void	OutPut() {
		lock_guard<mutex> lock(m_mutex);
		SetWindowText(g_hWnd, m_szFPS);
	}

	void	UpdateLoadingText(const _tchar* pText) {
		lock_guard<mutex> lock(m_mutex);
		lstrcpy(m_szFPS, pText);
	}

private:
	HRESULT		Loading_Logo();
	HRESULT		Loading_GamePlay();

private:
	LEVELID					m_eLoadingLevelID = { LEVELID::END };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CGameInstance*			m_pGameInstance = { nullptr };

	HANDLE				m_hThread = {};
	CRITICAL_SECTION	m_CriticalSection = {};

	thread				m_Thread = {};
	mutex				m_mutex = {};

	//_bool				m_bIsFinished = { false };
	atomic<_bool>		m_bIsFinished = { false };
	_tchar				m_szFPS[MAX_PATH] = {};

public:
	static CLoader* Create(LEVELID eNextLevelID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END