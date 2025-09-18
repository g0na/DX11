#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLevelManager final : public CBase
{
private:
	explicit CLevelManager();
	virtual ~CLevelManager() = default;

public:
	HRESULT	Change_Level(_uint iCurLevelID, class CLevel* pCurLevel);
	void Update_Level(_float fTimeDelta);
	HRESULT Render_Level();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };
	class CLevel*			m_pCurLevel = { nullptr };
	_uint					m_iCurLevelID = {};

public:
	static CLevelManager* Create();
	virtual void Free() override;
};

NS_END