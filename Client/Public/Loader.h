#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVELID eLoadingLevelID);
	HRESULT Loading();

	_bool	isFinished() const { return m_bIsFinished; }
	
	void	OutPut() { SetWindowText(g_hWnd, m_szFPS); }

private:
	HRESULT		Loading_Logo();
	HRESULT		Loading_GamePlay();

private:
	LEVELID					m_eLoadingLevelID = { LEVELID::END };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	HANDLE				m_hThread = {};
	CRITICAL_SECTION	m_CriticalSection = {};

	_bool				m_bIsFinished = { false };
	_tchar				m_szFPS[MAX_PATH] = {};

public:
	static CLoader* Create(LEVELID eNextLevelID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END