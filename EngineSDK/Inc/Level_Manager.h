#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	explicit CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT	Change_Level(_uint iCurLevelID, class CLevel* pCurLevel);
	void Update_Level(_float fTimeDelta);
	HRESULT Render_Level();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CLevel*			m_pCurLevel = { nullptr };
	_uint					m_iCurLevelID = {};

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

NS_END