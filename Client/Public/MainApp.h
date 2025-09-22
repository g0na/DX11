#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp();

public:
	HRESULT Initialize();
	void Update(const _float& fTimeDelta);
	HRESULT Render();
	HRESULT Start_Level(LEVELID eLevelID);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	EngineDesc	m_tEngineDesc = {};

public:
	static CMainApp* Create();
	virtual void Free() override;
};

NS_END