#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract :  public CBase
{
private:
	explicit CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

NS_END