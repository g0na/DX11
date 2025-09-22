#pragma once
#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI();

private:
	LEVELID						m_eNextLevelID = { LEVELID::END };
	class CLoader*				m_pLoader = { nullptr };

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

NS_END